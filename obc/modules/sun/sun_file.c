#include "sun_file.h"

#include "obc_reliance_fs.h"
#include "obc_logging.h"

#include "redposix.h"

#define SUN_FILE_HEADER_SIZE 20U
#define SUN_FILE_DATA_POINT_SIZE 12U

// TODO: Handle if the module is not initialized

// Cache data
static julian_date_t minJD = -1;
static julian_date_t maxJD = -1;
static double stepSize = 1;
static uint32_t numberOfDataPoints = 0;

// File related data
static int32_t fileID;
static const char *fileName = "sunFile.bin";

// Private Functions

/**
 * @breif Gets the jd of the index and stores it into jd
 * @attention jd must be a valid pointer
 */
static obc_error_code_t sunFileJDOfIndex(uint32_t index, julian_date_t *jd) {
  if (jd == NULL) {
    return OBC_ERROR_CODE_INVALID_ARGUMENT;
  }
  *jd = minJD + index * stepSize;
  return OBC_ERROR_CODE_SUCCESS;
}

// Maybe this should be put into obc_reliance_fs instead
/**
 * @breif Sets the file of set to the location
 * @return OBC_ERROR_CODE_SUCCESS if successful, otherwise an error code
 */
static obc_error_code_t sunFileSeek(int64_t location) {
  int64_t offSet = red_lseek(fileID, location, RED_SEEK_SET);
  if (offSet < 0) {
    LOG_ERROR_CODE(red_errno + RELIANCE_EDGE_ERROR_CODES_OFFSET);
    return OBC_ERROR_CODE_INVALID_STATE;  // TODO: Change this
  }
  return OBC_ERROR_CODE_SUCCESS;
}

/**
 * @breif Reads in a position_t from the file into buff
 * @attention Buff must be valid pointer
 * @return OBC_ERROR_CODE_SUCCESS if successful, otherwise an error code
 */
static obc_error_code_t sunFileReadPosition(position_t *buff) {
  if (buff == NULL) return OBC_ERROR_CODE_INVALID_ARGUMENT;
  size_t bytesRead;
  position_t tmp;
  RETURN_IF_ERROR_CODE(readFile(fileID, &tmp, sizeof(position_t), &bytesRead));
  if (bytesRead != sizeof(position_t)) return OBC_ERROR_CODE_INVALID_STATE;
  *buff = tmp;
  return OBC_ERROR_CODE_SUCCESS;
}

// Public Functions

obc_error_code_t sunFileInit(void) {
  // Init file

  // Set offset to beginning
  RETURN_IF_ERROR_CODE(sunFileSeek(0));
  size_t length;

  // Read minimum jd
  julian_date_t minimumJD;
  RETURN_IF_ERROR_CODE(readFile(fileID, &minimumJD, sizeof(julian_date_t), &length))
  if (length != sizeof(julian_date_t)) return OBC_ERROR_CODE_INVALID_STATE;

  // Read step size
  double stepSizeBuffer;
  RETURN_IF_ERROR_CODE(readFile(fileID, &stepSizeBuffer, sizeof(double), &length))
  if (length != sizeof(double)) return OBC_ERROR_CODE_INVALID_STATE;

  // Read number of data points
  uint32_t numDataPoints;
  RETURN_IF_ERROR_CODE(readFile(fileID, &numDataPoints, sizeof(uint32_t), &length))
  if (length != sizeof(uint32_t)) return OBC_ERROR_CODE_INVALID_STATE;

  RETURN_IF_ERROR_CODE(numberOfDataPoints, &maxJD);
  return OBC_ERROR_CODE_SUCCESS;
}

obc_error_code_t sunFileGetMinJD(julian_date_t *jd) {
  if (jd == NULL) {
    return OBC_ERROR_CODE_INVALID_ARGUMENT;
  }
  *jd = minJD;
  return OBC_ERROR_CODE_SUCCESS;
}

obc_error_code_t sunFileGetMaxJD(julian_date_t *jd);
if (jd == NULL) {
  return OBC_ERROR_CODE_INVALID_ARGUMENT;
}
*jd = maxJD;
return OBC_ERROR_CODE_SUCCESS;
}

obc_error_code_t sunFileJDInRange(julian_date_t jd, bool *buff) {
  if (buff == NULL) {
    return OBC_ERROR_CODE_INVALID_ARGUMENT;
  }
  julian_date_t minimumJD, maximumJD;
  RETURN_IF_ERROR_CODE(sunFileGetMinJD(&minJD));
  RETURN_IF_ERROR_CODE(sunFileGetMaxJD(&maxJD));
  *buff = (minJD <= jd) && (jd <= maxJD);
  return OBC_ERROR_CODE_SUCCESS;
}

obc_error_code_t sunFileReadDataPoint(uint32_t index, position_data_t *buff) {
  if (buff == NULL) {
    return OBC_ERROR_CODE_INVALID_ARGUMENT;
  }
  RETURN_IF_ERROR_CODE(sunFileSeek(SUN_FILE_HEADER_SIZE + index * SUN_FILE_DATA_POINT_SIZE));
  julian_date_t jd;
  RETURN_IF_ERROR_CODE(sunFileJDOfIndex(index, &jd));
  position_t x, y, z;
  RETURN_IF_ERROR_CODE(sunFileReadPosition(&x));
  RETURN_IF_ERROR_CODE(sunFileReadPosition(&y));
  RETURN_IF_ERROR_CODE(sunFileReadPosition(&z));
  buff->julianDate = jd;
  buff->x = x;
  buff->y = y;
  buff->z = z;
  return OBC_ERROR_CODE_SUCCESS;
}

obc_error_code_t sunFileGetNumDataPoints(uint32_t *number) {
  if (number == NULL) {
    return OBC_ERROR_CODE_INVALID_ARGUMENT;
  }
  *number = numberOfDataPoints;
  return OBC_ERROR_CODE_SUCCESS;
}

obc_error_code_t sunFileGetIndexOfJD(julian_date_t jd, uint32_t *index) {
  if (stepSize == 0) {
    return OBC_ERROR_CODE_INVALID_STATE;
  }

  if (index == NULL) {
    return OBC_ERROR_CODE_INVALID_ARGUMENT;
  }

  bool isInRange = false;
  RETURN_IF_ERROR_CODE(sunFileJDInRange(jd, &isInRange));
  if (!isInRange) {
    return OBC_ERROR_CODE_INVALID_ARGUMENT;
  }

  *index = (jd - minJD) / stepSize;
  return OBC_ERROR_CODE_SUCCESS;
}

obc_error_code_t sunFileGetNumDataPointsAfterJD(julian_date_t jd, uint32_t *number) {
  if (number == NULL) {
    return OBC_ERROR_CODE_INVALID_ARGUMENT;
  }
  uint32_t index;
  RETURN_IF_ERROR_CODE(sunFileGetIndexOfJD(jd, &index));
  *number = numberOfDataPoints - index - 1;
  return OBC_ERROR_CODE_SUCCESS;
}
