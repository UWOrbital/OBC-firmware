/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *
%%Copyright%%
 *
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------
 *
 *      Project:  Hercules� ARM� Safety MCUs - F021 Flash API
 *      Version:  v2.01.01 Build(000830)
 *   Build Date:  2014-10-21
 *
%%File%%
 *
 *  Description:  Contains all user defined callback functions used by the F021 Flash API.
 *---------------------------------------------------------------------------------------------------------------------
 * Author:  John R Hall
 *---------------------------------------------------------------------------------------------------------------------
 *
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * INCLUDES
 *********************************************************************************************************************/
#include "F021.h"

/*LDRA_HEADER_END*/

/**********************************************************************************************************************
 *  Fapi_serviceWatchdogTimer
 *********************************************************************************************************************/
/*! Callback function to service watchdog timer.  Used by the Blank, Read, and Verify fuctions.
 *
 *  \param [in]  none
 *  \param [out] none
 *  \return     Fapi_StatusType
 *  \retval     Fapi_Status_Success
 *  \note       TI FEE API.
 *********************************************************************************************************************/

Fapi_StatusType Fapi_serviceWatchdogTimer(void) {
  /* User to add their own watchdog servicing code here */
  return (Fapi_Status_Success);
}

/**********************************************************************************************************************
 *  END OF FILE: Fapi_UserDefinedFunctions.c
 *********************************************************************************************************************/
