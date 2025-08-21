#include "sun_file.h"

#include "obc_reliance_fs.h"
#include "obc_logging.h"

#include <redposix.h>

#define SUN_FILE_HEADER_SIZE 20U
#define SUN_FILE_DATA_POINT_SIZE 32U

// Cache data
static julian_date_t minJD = -1;
static julian_date_t maxJD = -1;
static double stepSize = 1;
static uint32_t numberOfDataPoints = 0;  // Number of data points can never be 0, serves as check for initialized

// File related data
static int32_t fileID = -1;

// Private Functions

/**
 * @breif Gets the jd of the index and stores it into jd
 * @attention jd must be a valid pointer and the index is less than the numberOfDataPoints
 */
static obc_error_code_t sunFileJDOfIndex(uint32_t index, julian_date_t *jd) {
  if (jd == NULL || index >= numberOfDataPoints) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  *jd = minJD + index * stepSize;
  return OBC_ERR_CODE_SUCCESS;
}

// Maybe something similar should be put into obc_reliance_fs instead
/**
 * @breif Sets the file of set to the location
 * @return OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
static obc_error_code_t sunFileSeek(int64_t location) {
  obc_error_code_t errCode;
  int64_t offSet = red_lseek(fileID, location, RED_SEEK_SET);
  //sciPrintf("offSet:%lld\r\n", offSet);
  if (offSet < 0) {
    LOG_IF_ERROR_CODE(red_errno + RELIANCE_EDGE_ERROR_CODES_OFFSET);
    return OBC_ERR_CODE_INVALID_STATE;
  }
  return OBC_ERR_CODE_SUCCESS;
}

/**
 * @breif Reads in a position_t from the file into buff
 * @attention Buff must be valid pointer
 * @return OBC_ERR_CODE_SUCCESS if successful, otherwise an error code
 */
static obc_error_code_t sunFileReadPosition(position_t *buff) {
  if (buff == NULL) return OBC_ERR_CODE_INVALID_ARG;
  obc_error_code_t errCode;
  size_t bytesRead;
  double tmp;
  RETURN_IF_ERROR_CODE(readFile(fileID, &tmp, sizeof(double), &bytesRead));
  if (bytesRead != sizeof(double)) return OBC_ERR_CODE_INVALID_STATE;
  *buff = (position_t)tmp;
  return OBC_ERR_CODE_SUCCESS;
}

// Public Functions

obc_error_code_t sunFileInit(const char *fileName) {
  obc_error_code_t errCode;
  // Init file
  //f("sunFileInit - createFile\r\n");
  RETURN_IF_ERROR_CODE(createFile(fileName, &fileID));

  RETURN_IF_ERROR_CODE(openFile(fileName, RED_O_RDONLY, &fileID));

  // Set offset to beginning
  size_t length;
  //sciPrintf("sunFileInit - sunFileSeek\r\n");
  RETURN_IF_ERROR_CODE(sunFileSeek(0));

  // Read minimum jd
  julian_date_t minimumJD;
  //sciPrintf("sunFileInit - readFile\r\n");
  RETURN_IF_ERROR_CODE(readFile(fileID, &minimumJD, sizeof(julian_date_t), &length));
  // sciPrintf("length: %zu != sizeof(julian_date_t): %zu \r\n", length, sizeof(julian_date_t));
  // sciPrintf("test\r\n");
  if (length != sizeof(julian_date_t)) return OBC_ERR_CODE_INVALID_STATE;

  // Read step size
  double stepSizeBuffer;
  //sciPrintf("test 2\r\n");
  RETURN_IF_ERROR_CODE(readFile(fileID, &stepSizeBuffer, sizeof(double), &length));
  //sciPrintf("length != sizeof(double)\r\n");
  if (length != sizeof(double)) return OBC_ERR_CODE_INVALID_STATE;

  // Read number of data points
  uint32_t numDataPoints;
  RETURN_IF_ERROR_CODE(readFile(fileID, &numDataPoints, sizeof(uint32_t), &length));
  //RETURN_IF_ERROR_CODE(closeFile(fileID));
  if (length != sizeof(uint32_t) || numDataPoints == 0) return OBC_ERR_CODE_INVALID_STATE;

  
  minJD = minimumJD;
  stepSize = stepSizeBuffer;
  numberOfDataPoints = numDataPoints;
  maxJD = minJD + (numberOfDataPoints - 1) * stepSize;

  // debugging
  //sciPrintf("minJD: %lf, stepSize: %lf, numberOfDataPoints: %u, maxJD: %lf \r\n", minJD, stepSize, numberOfDataPoints, maxJD);
  //uint8_t* bytes = (uint8_t*)&minimumJD;
  //sciPrintf("minimumJD bytes: %02X %02X %02X %02X %02X %02X %02X %02X\r\n", 
          //bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7]);

  RETURN_IF_ERROR_CODE(sunFileJDOfIndex(numDataPoints - 1, &maxJD));
  
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunFileGetMinJD(julian_date_t *jd) {
  if (jd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  *jd = minJD;
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunFileGetMaxJD(julian_date_t *jd) {
  if (jd == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  *jd = maxJD;
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunFileJDInRange(julian_date_t jd, bool *buff) {
  if (buff == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  *buff = (minJD <= jd) && (jd <= maxJD);
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t testRead() {
  obc_error_code_t errCode;

  RETURN_IF_ERROR_CODE("sunFileSeek", sunFileSeek(SUN_FILE_HEADER_SIZE));

  uint8_t raw[12];
  size_t bytesRead;
  STOP_ON_ERROR("readFile", readFile(fileID, raw, sizeof(raw), &bytesRead));
  sciPrintf("Raw bytes after header");
  for (int i = 0; i < 12; ++i) sciPrintf("%02X ", raw[i]);
  sciPrintf("\r\n");

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunFileReadDataPoint(uint32_t index, position_data_t *buff) {
  obc_error_code_t errCode;
  //sciPrintf("check");
  if (buff == NULL || index >= numberOfDataPoints) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  julian_date_t jd;
  RETURN_IF_ERROR_CODE(sunFileJDOfIndex(index, &jd));
  RETURN_IF_ERROR_CODE(sunFileSeek(SUN_FILE_HEADER_SIZE + index * SUN_FILE_DATA_POINT_SIZE));



  
  //sciPrintf("RETURN_IF_ERROR_CODE(sunFileReadPosition(&x));\r\n");
  double fileJD;
  size_t bytesRead;
  RETURN_IF_ERROR_CODE(readFile(fileID, &fileJD, sizeof(double), &bytesRead));
  position_t x, y, z;
  RETURN_IF_ERROR_CODE(sunFileReadPosition(&x));
  RETURN_IF_ERROR_CODE(sunFileReadPosition(&y));
  RETURN_IF_ERROR_CODE(sunFileReadPosition(&z));
  buff->julianDate = jd;
  buff->x = x;
  buff->y = y;
  buff->z = z;
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunFileGetNumDataPoints(uint32_t *number) {
  if (number == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  *number = numberOfDataPoints;
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunFileGetIndexOfJD(julian_date_t jd, uint32_t *index) {
  if (stepSize == 0) {
    return OBC_ERR_CODE_INVALID_STATE;
  }

  if (index == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  bool isInRange = false;
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sunFileJDInRange(jd, &isInRange));
  if (!isInRange) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  *index = (jd - minJD) / stepSize;
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunFileGetNumDataPointsAfter(julian_date_t jd, uint32_t *number) {
  obc_error_code_t errCode;

  if (number == NULL) {
    return OBC_ERR_CODE_INVALID_ARG;
  }

  uint32_t index;
  RETURN_IF_ERROR_CODE(sunFileGetIndexOfJD(jd, &index));
  uint32_t totalDataPoints;
  RETURN_IF_ERROR_CODE(sunFileGetNumDataPoints(&totalDataPoints));

  if (index + 1 >= totalDataPoints) {
    *number = 0;
  } else {
    *number = totalDataPoints - (index + 1);
  }

  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunFileWriteHeader(julian_date_t minimumJD, double stepSize, uint32_t numDataPoints) {
  obc_error_code_t errCode;
  RETURN_IF_ERROR_CODE(sunFileSeek(0));
  RETURN_IF_ERROR_CODE(writeFile(fileID, &minimumJD, sizeof(julian_date_t)));
  RETURN_IF_ERROR_CODE(writeFile(fileID, &stepSize, sizeof(double)));
  RETURN_IF_ERROR_CODE(writeFile(fileID, &numDataPoints, sizeof(uint32_t)));
  return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t sunFileWriteDataPoint(uint32_t index, position_data_t *buff) {
  obc_error_code_t errCode;
  if (buff == NULL || index >= numberOfDataPoints) {
    return OBC_ERR_CODE_INVALID_ARG;
  }
  RETURN_IF_ERROR_CODE(sunFileSeek(SUN_FILE_HEADER_SIZE + index * SUN_FILE_DATA_POINT_SIZE));
  float x = (float)buff->x;
  float y = (float)buff->y;
  float z = (float)buff->z;
  RETURN_IF_ERROR_CODE(writeFile(fileID, &x, sizeof(float)));
  RETURN_IF_ERROR_CODE(writeFile(fileID, &y, sizeof(float)));
  RETURN_IF_ERROR_CODE(writeFile(fileID, &z, sizeof(float)));
  return OBC_ERR_CODE_SUCCESS;
}
