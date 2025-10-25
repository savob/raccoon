/*******************************************************************************
  Touch Library 4.2.0 Release

  @Company
    Microchip Technology Inc.

  @File Name
    touch.c

  @Summary
    QTouch Modular Library

  @Description
    Provides Initialization, Processing and ISR handler of touch library,
          Simple API functions to get/set the key touch parameters from/to the
          touch library data structures.
	
*******************************************************************************/
/*
    MICROCHIP SOFTWARE NOTICE AND DISCLAIMER:

    Subject to your compliance with the terms below and any license terms accompanying this software, you may use this software, and any derivatives created by any person or
    entity by or on your behalf, exclusively with Microchip's products.
    Microchip and its subsidiaries ("Microchip"), and its licensors, retain all
    ownership and intellectual property rights in the accompanying software and
    in all derivatives hereto.

    This software and any accompanying information is for suggestion only. It
    does not modify Microchip's standard warranty for its products.  You agree
    that you are solely responsible for testing the software and determining
    its suitability.  Microchip has no obligation to modify, test, certify, or
    support the software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE APPLY TO THIS SOFTWARE, ITS INTERACTION WITH MICROCHIP'S
    PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT, WILL MICROCHIP BE LIABLE, WHETHER IN CONTRACT, WARRANTY, TORT
    (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), STRICT LIABILITY,
    INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF
    ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWSOEVER CAUSED, EVEN IF
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
    FORESEEABLE.  TO THE FULLEST EXTENT ALLOWABLE BY LAW, MICROCHIP'S TOTAL
    LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
    THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
    THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF
    THESE TERMS.
*/

#ifndef TOUCH_C
#define TOUCH_C
/*----------------------------------------------------------------------------
 *     include files
 *----------------------------------------------------------------------------*/
#include "touch.h" 
#include "../timer/rtc.h"
#if DEF_TOUCH_DATA_STREAMER_ENABLE == 0u
#if DEF_PTC_CAL_OPTION != CAL_AUTO_TUNE_NONE
#warning "Automatic charge time tuning option is enabled without enabling datastreamer. So, automatic charge time tuning option is disabled."
#define DEF_PTC_CAL_OPTION CAL_AUTO_TUNE_NONE
#endif
#endif

/*----------------------------------------------------------------------------
 *   prototypes
 *----------------------------------------------------------------------------*/

/*! \brief configure keys, wheels and sliders.
 */
static touch_ret_t touch_sensors_config(void);

/*! \brief Touch measure complete callback function example prototype.
 */
static void qtm_measure_complete_callback(void);

/*! \brief Touch Error callback function prototype.
 */
static void qtm_error_callback(uint8_t error);

#if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
/* low power processing function */
static void touch_process_lowpower();
/* low power touch detection callback */
static void touch_measure_wcomp_match(void);
/* Cancel low-power measurement */
static void touch_cancel_autoscan(void);
static void touch_enable_lowpower_measurement(void);
static void touch_disable_lowpower_measurement(void);

#endif
/*----------------------------------------------------------------------------
 *     Global Variables
 *----------------------------------------------------------------------------*/

/* Flag to indicate time for touch measurement */
volatile uint8_t time_to_measure_touch_flag = 0;
/* post-process request flag */
volatile uint8_t touch_postprocess_request = 0;

/* Measurement Done Touch Flag  */
volatile uint8_t measurement_done_touch = 0;

/* Error Handling */
uint8_t module_error_code = 0;

/* Acquisition module internal data - Size to largest acquisition set */
uint16_t touch_acq_signals_raw[DEF_NUM_CHANNELS];
/* Low-power measurement variables */
uint16_t time_since_touch = 0;
/* store the drift period for comparison */
uint16_t measurement_period_store = 0;

/* Acquisition set 1 - General settings */
qtm_acq_node_group_config_t ptc_qtlib_acq_gen1
    = {DEF_NUM_CHANNELS, DEF_SENSOR_TYPE, DEF_PTC_CAL_AUTO_TUNE, DEF_SEL_FREQ_INIT};

/* Node status, signal, calibration values */
qtm_acq_node_data_t ptc_qtlib_node_stat1[DEF_NUM_CHANNELS];
/* Node configurations */
qtm_acq_t161x_node_config_t ptc_seq_node_cfg1[DEF_NUM_CHANNELS] = {NODE_0_PARAMS};
/* Container */
qtm_acquisition_control_t qtlib_acq_set1 = {&ptc_qtlib_acq_gen1, &ptc_seq_node_cfg1[0], &ptc_qtlib_node_stat1[0]};
/* Low-power autoscan related parameters */
qtm_auto_scan_config_t auto_scan_setup = {&qtlib_acq_set1,QTM_AUTOSCAN_NODE,QTM_AUTOSCAN_THRESHOLD,QTM_AUTOSCAN_TRIGGER_PERIOD};
/**********************************************************/
/*********************** Keys Module **********************/
/**********************************************************/

/* Keys set 1 - General settings */
qtm_touch_key_group_config_t qtlib_key_grp_config_set1 = {DEF_NUM_SENSORS,
                                                          DEF_TOUCH_DET_INT,
                                                          DEF_MAX_ON_DURATION,
                                                          DEF_ANTI_TCH_DET_INT,
                                                          DEF_ANTI_TCH_RECAL_THRSHLD,
                                                          DEF_TCH_DRIFT_RATE,
                                                          DEF_ANTI_TCH_DRIFT_RATE,
                                                          DEF_DRIFT_HOLD_TIME,
                                                          DEF_REBURST_MODE};

qtm_touch_key_group_data_t qtlib_key_grp_data_set1;

/* Key data */
qtm_touch_key_data_t qtlib_key_data_set1[DEF_NUM_SENSORS];

/* Key Configurations */
qtm_touch_key_config_t qtlib_key_configs_set1[DEF_NUM_SENSORS] = {KEY_0_PARAMS}; 
/* Container */
qtm_touch_key_control_t qtlib_key_set1
    = {&qtlib_key_grp_data_set1, &qtlib_key_grp_config_set1, &qtlib_key_data_set1[0], &qtlib_key_configs_set1[0]};



/*----------------------------------------------------------------------------
 *   function definitions
 *----------------------------------------------------------------------------*/


/*============================================================================
static touch_ret_t touch_sensors_config(void)
------------------------------------------------------------------------------
Purpose: Initialization of touch key sensors
Input  : none
Output : none
Notes  :
============================================================================*/
/* Touch sensors config - assign nodes to buttons / wheels / sliders / surfaces / water level / etc */
static touch_ret_t touch_sensors_config(void)
{
    uint16_t    sensor_nodes;
    touch_ret_t touch_ret = TOUCH_SUCCESS;
    /* Init acquisition module */
    qtm_ptc_init_acquisition_module(&qtlib_acq_set1);
    /* Init pointers to DMA sequence memory */

    qtm_ptc_qtlib_assign_signal_memory(&touch_acq_signals_raw[0]);

    /* Initialize sensor nodes */
	for (sensor_nodes = 0u; sensor_nodes < DEF_NUM_CHANNELS; sensor_nodes++) {
        /* Enable each node for measurement and mark for calibration */
        qtm_enable_sensor_node(&qtlib_acq_set1, sensor_nodes);
        qtm_calibrate_sensor_node(&qtlib_acq_set1, sensor_nodes);
    }
    /* Enable sensor keys and assign nodes */
    for (sensor_nodes = 0u; sensor_nodes < DEF_NUM_SENSORS; sensor_nodes++) {
			qtm_init_sensor_key(&qtlib_key_set1, sensor_nodes, &ptc_qtlib_node_stat1[sensor_nodes]);
    }
    


    return (touch_ret);
}

/*============================================================================
static void qtm_measure_complete_callback( void )
------------------------------------------------------------------------------
Purpose: Callback function called after the completion of
         measurement cycle. This function sets the post processing request
         flag to trigger the post processing.
Input  : none
Output : none
Notes  :
============================================================================*/
static void qtm_measure_complete_callback(void)
{
    touch_postprocess_request = 1u;
}


/*============================================================================
static void qtm_error_callback(uint8_t error)
------------------------------------------------------------------------------
Purpose: Callback function called after the completion of
         post processing. This function is called only when there is error.
Input  : error code
Output : decoded module error code
Notes  :
Derived Module_error_codes:
    Acquisition module error =1
    post processing module1 error = 2
    post processing module2 error = 3
    ... and so on

============================================================================*/
static void qtm_error_callback(uint8_t error)
{
	module_error_code = error + 1u;

}
/*============================================================================
void Timer_set_period(const uint16_t val)
------------------------------------------------------------------------------
Purpose: This function sets the time interval on the RTC/Timer peripheral based
         on the user configuration.
Input  : Time interval
Output : none
Notes  :
============================================================================*/
void Timer_set_period(const uint16_t val)
{

	if (val != 0) {
		while (RTC.STATUS & RTC_PERBUSY_bm)
			;          /* wait for RTC synchronization */
		RTC.PER = val; /* Set period register */
		RTC.INTCTRL |= (1 << RTC_OVF_bp);
	} else { /* if the value is zero, disable interrupt */
		RTC.INTCTRL &= ~(1 << RTC_OVF_bp);
	}

	while (RTC.STATUS & RTC_CNTBUSY_bm)
		;         /* wait for RTC synchronization */
	RTC.CNT = 0u; /* Clear count register */
}
/*============================================================================
void touch_init(void)
------------------------------------------------------------------------------
Purpose: Initialization of touch library. PTC, timer, and
         datastreamer modules are initialized in this function.
Input  : none
Output : none
Notes  :
============================================================================*/
void touch_init(void)
{
    Timer_set_period(DEF_TOUCH_MEASUREMENT_PERIOD_MS);
	RTC_SetOVFIsrCallback(touch_timer_handler);
#if DEF_TOUCH_LOWPOWER_ENABLE == 1u
	touch_disable_lowpower_measurement();
#endif
	/* Configure touch sensors with Application specific settings */
	touch_sensors_config();
	
}
/*============================================================================
static void touch_process_lowpower(void)
------------------------------------------------------------------------------
Purpose: Processing function for low-power touch measurment.
         Monitors the touch activity and if there is not touch for longer
         duration, low-power autoscan is enabled.
Input  : none
Output : none
Notes  :
============================================================================*/
static void touch_process_lowpower(void)
{
	touch_ret_t touch_ret;

	if (time_since_touch >= DEF_TOUCH_TIMEOUT) {

		/* Start Autoscan */
		touch_ret = qtm_autoscan_sensor_node(&auto_scan_setup, touch_measure_wcomp_match);

		if ((measurement_period_store != DEF_TOUCH_DRIFT_PERIOD_MS) && (touch_ret == TOUCH_SUCCESS)) {

			/* Enable Event System */
			touch_enable_lowpower_measurement();
		}
	} else if (measurement_period_store != DEF_TOUCH_MEASUREMENT_PERIOD_MS) {

		/* Cancel node auto scan */
		qtm_autoscan_node_cancel();

		/* disable event system measurement */
		touch_disable_lowpower_measurement();
	}
}
/*============================================================================
static void touch_measure_wcomp_match(void)
------------------------------------------------------------------------------
Purpose: callback of autoscan function
Input  : none
Output : none
Notes  :
============================================================================*/
void touch_measure_wcomp_match(void)
{
    if(measurement_period_store != DEF_TOUCH_MEASUREMENT_PERIOD_MS) {
        touch_cancel_autoscan();
		time_to_measure_touch_flag = 1u;
        time_since_touch = 0u;
    }
}
/*============================================================================
static void touch_cancel_autoscan(void)
------------------------------------------------------------------------------
Purpose: cancelling of autoscan functionality
Input  : none
Output : none
Notes  :
============================================================================*/
static void touch_cancel_autoscan(void)
{
        /* disable event system measurement */
        touch_disable_lowpower_measurement();
}
void touch_enable_lowpower_measurement(void)
{
/* wait for RTC synchronization and Disable Clock */
	while (RTC.STATUS & RTC_CTRLABUSY_bm)
		;
	RTC.CTRLA &= ~(1 << RTC_RTCEN_bp);

	/* Set match value for timer */
	measurement_period_store = DEF_TOUCH_DRIFT_PERIOD_MS;

	/* Enable RTC to PTC Event system and Set drift wakeup period*/
	if (auto_scan_setup.auto_scan_trigger < NODE_SCAN_64MS) {
		/* Set source of event channel with autoscan Periodic Interval  */
		EVSYS.ASYNCCH3   = (EVSYS_ASYNCCH3_PIT_DIV64_gc - auto_scan_setup.auto_scan_trigger);
		EVSYS.ASYNCUSER1 = EVSYS_ASYNCUSER1_ASYNCCH3_gc; /* Asynchronous Event Channel 3 */

		/* Set match value for timer */
		/* 32KHzclock is divided by 8 resulting to 4KHz; so multiply by 4. */
		Timer_set_period(measurement_period_store << 2);
	} else {
		/* Set source of event channel with autoscan Periodic Interval  */
		EVSYS.ASYNCCH3   = (EVSYS_ASYNCCH3_PIT_DIV64_gc - auto_scan_setup.auto_scan_trigger) + 5u;
		EVSYS.ASYNCUSER1 = EVSYS_ASYNCUSER1_ASYNCCH3_gc; /* Asynchronous Event Channel 3 */

		Timer_set_period(measurement_period_store);
	}

	/* Enable Clock and PIT */
	while (RTC.PITSTATUS & RTC_CTRLBUSY_bm)
		;
	RTC.PITCTRLA |= 1 << RTC_PITEN_bp;
	while (RTC.STATUS & RTC_CTRLABUSY_bm)
		;
	RTC.CTRLA |= 1 << RTC_RTCEN_bp;
}
void touch_disable_lowpower_measurement(void)
{
    /* Disable RTC to PTC Event system */
	EVSYS.ASYNCCH3   = EVSYS_ASYNCCH3_OFF_gc;
	EVSYS.ASYNCUSER1 = EVSYS_ASYNCUSER1_OFF_gc;

	/* Disable Clock and PIT */
	while (RTC.STATUS & RTC_CTRLABUSY_bm)
		;                               /* wait for RTC synchronization */
	RTC.CTRLA &= ~(1u << RTC_RTCEN_bp); /* Clock disabled */
	while (RTC.PITSTATUS & RTC_CTRLBUSY_bm)
		;                                  /* wait for RTC synchronization */
	RTC.PITCTRLA &= ~(1u << RTC_PITEN_bp); /* PIT disabled */

	measurement_period_store = DEF_TOUCH_MEASUREMENT_PERIOD_MS;

	/* Enable RTC to PTC Event system and Set drift wakeup period*/
	if (auto_scan_setup.auto_scan_trigger < NODE_SCAN_64MS) {
		/* Select the 32kHz internal clock */
		RTC.CLKSEL = RTC_CLKSEL_INT32K_gc; /* Internal 32kHz OSC */
		/* Enable clock, 32KHzclock is divided by 8 resulting to 4KHz */
		while (RTC.STATUS & RTC_CTRLABUSY_bm)
			;                               /* wait for RTC synchronization */
		RTC.CTRLA = RTC_PRESCALER_DIV8_gc   /* 8 */
		            | 1 << RTC_RTCEN_bp     /* Enable: enabled */
		            | 1 << RTC_RUNSTDBY_bp; /* Run In Standby: disabled */
		/* Set match value for timer */
		/* 32KHzclock is divided by 8 resulting to 4KHz; so multiply by 4. */
		Timer_set_period(measurement_period_store << 2);
	} else {
		/* Select the 1kHz internal clock */
		RTC.CLKSEL = RTC_CLKSEL_INT1K_gc; /* Internal 1kHz OSC */
		/* Enable clock,, 1KHzclock is divided by 1 resulting to 1KHz */
		while (RTC.STATUS & RTC_CTRLABUSY_bm)
			;                               /* wait for RTC synchronization */
		RTC.CTRLA = RTC_PRESCALER_DIV1_gc   /* 1 */
		            | 1 << RTC_RTCEN_bp     /* Enable: enabled */
		            | 1 << RTC_RUNSTDBY_bp; /* Run In Standby: enabled */
		/* Set match value for timer */
		Timer_set_period(measurement_period_store);
	}

	/* Enable Clock */
	while (RTC.STATUS & RTC_CTRLABUSY_bm)
		;                           /* wait for RTC synchronization */
	RTC.CTRLA |= 1 << RTC_RTCEN_bp; /* clock Enabled */
    
}
/*============================================================================
void touch_process(void)
------------------------------------------------------------------------------
Purpose: Main processing function of touch library. This function initiates the
         acquisition, calls post processing after the acquistion complete and
         sets the flag for next measurement based on the sensor status.
Input  : none
Output : none
Notes  :
============================================================================*/
void touch_process(void)
{
    touch_ret_t touch_ret;
    /* check the time_to_measure_touch for Touch Acquisition */
    if (time_to_measure_touch_flag == 1u)
	{
        /* Do the acquisition */
         touch_ret = qtm_ptc_start_measurement_seq(&qtlib_acq_set1, qtm_measure_complete_callback);

        /* if the Acquistion request was successful then clear the request flag */
        if (TOUCH_SUCCESS == touch_ret) {
            /* Clear the Measure request flag */
			time_to_measure_touch_flag = 0u;
        }
    }
    /* check the flag for node level post processing */
    if (touch_postprocess_request == 1u){
        /* Reset the flags for node_level_post_processing */
        touch_postprocess_request = 0u;
        /* Run Acquisition module level post processing*/
        touch_ret = qtm_acquisition_process();
        /* Check the return value */
        if (TOUCH_SUCCESS == touch_ret) {
            /* Returned with success: Start module level post processing */

            touch_ret = qtm_key_sensors_process(&qtlib_key_set1);
            if (TOUCH_SUCCESS != touch_ret) {
                qtm_error_callback(1);
           }





         }else {
           /* Acq module Error Detected: Issue an Acq module common error code 0x80 */
            qtm_error_callback(0);
        }

#if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
        if (0u != (qtlib_key_grp_data_set1.qtm_keys_status & QTM_KEY_DETECT)) {
            /* Something in detect */
            time_since_touch = 0u;
        }
#endif

        if((0u != (qtlib_key_set1.qtm_touch_key_group_data->qtm_keys_status & 0x80u)))
        {
        time_to_measure_touch_flag = 1u;
        } else {
            measurement_done_touch = 1u;
#if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
            /* process lowpower touch measurement */
            touch_process_lowpower();
#endif
        }
    }

    

}
uint8_t interrupt_cnt;
/*============================================================================
void touch_timer_handler(void)
------------------------------------------------------------------------------
Purpose: This function updates the time elapsed to the touch key module to
         synchronize the internal time counts used by the module.
Input  : none
Output : none
Notes  :
============================================================================*/
void touch_timer_handler(void)
{
		/* Count complete - Measure touch sensors */
		time_to_measure_touch_flag = 1u;
#if DEF_TOUCH_LOWPOWER_ENABLE == 1u
	if (time_since_touch < (65535u - measurement_period_store)) {
		time_since_touch += measurement_period_store;
	} else {
		time_since_touch = 65535;
	}
    qtm_update_qtlib_timer(measurement_period_store);
#else
    qtm_update_qtlib_timer(DEF_TOUCH_MEASUREMENT_PERIOD_MS);
#endif
}
uint16_t get_sensor_node_signal(uint16_t sensor_node)
{
    return (ptc_qtlib_node_stat1[sensor_node].node_acq_signals);
}

void update_sensor_node_signal(uint16_t sensor_node, uint16_t new_signal)
{
    ptc_qtlib_node_stat1[sensor_node].node_acq_signals = new_signal;
}

uint16_t get_sensor_node_reference(uint16_t sensor_node)
{
    return (qtlib_key_data_set1[sensor_node].channel_reference);
}

void update_sensor_node_reference(uint16_t sensor_node, uint16_t new_reference)
{
    qtlib_key_data_set1[sensor_node].channel_reference = new_reference;
}

uint16_t get_sensor_cc_val(uint16_t sensor_node)
{
    return (ptc_qtlib_node_stat1[sensor_node].node_comp_caps);
}

void update_sensor_cc_val(uint16_t sensor_node, uint16_t new_cc_value)
{
    ptc_qtlib_node_stat1[sensor_node].node_comp_caps = new_cc_value;
}

uint8_t get_sensor_state(uint16_t sensor_node)
{
    return (qtlib_key_set1.qtm_touch_key_data[sensor_node].sensor_state);
}

void update_sensor_state(uint16_t sensor_node, uint8_t new_state)
{
    qtlib_key_set1.qtm_touch_key_data[sensor_node].sensor_state = new_state;
}

void calibrate_node(uint16_t sensor_node)
{
    /* Calibrate Node */
    qtm_calibrate_sensor_node(&qtlib_acq_set1, sensor_node);
    /* Initialize key */
    qtm_init_sensor_key(&qtlib_key_set1, sensor_node, &ptc_qtlib_node_stat1[sensor_node]);
}



/*============================================================================
ISR(ADC0_RESRDY_vect)
------------------------------------------------------------------------------
Purpose:  Interrupt handler for ADC / PTC EOC Interrupt
Input    :  none
Output  :  none
Notes    :  none
============================================================================*/
ISR(ADC0_RESRDY_vect)
{
	qtm_t161x_ptc_handler_eoc();
}
/*============================================================================
ISR(ADC0_WCOMP_vect)
------------------------------------------------------------------------------
Purpose:  Interrupt handler for ADC / PTC WCOMP Interrupt
Input    :  none
Output  :  none
Notes    :  none
============================================================================*/
ISR(ADC0_WCOMP_vect)
{
	qtm_t161x_ptc_handler_wcomp();
}
#endif /* TOUCH_C */