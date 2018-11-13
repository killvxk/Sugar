import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class MileageDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'Mileage')

    
    def __FixedData__(self, resultJson):
        result_mileage = self.__ParseData__(resultJson)

        logging.info(result_mileage + u' Fixed To ')
        
        confidencelevel, mileage = ConfidenceLevel.Bad, result_mileage

        logging.info(mileage)

        return confidencelevel, mileage


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_mileage = self.__ParseData__(resultJson)
        validated_mileage = self.__ParseData__(validateJson)

        if not self.__CheckData__(validated_mileage):
            logging.info(u'Validated Data Error')
            return

        if validated_mileage != result_mileage:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_mileage + u' Fixed To ')
        
        confidencelevel, result_mileage = ConfidenceLevel.Bad, result_mileage

        logging.info(result_mileage)

        if validated_mileage == result_mileage:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_mileage)
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        mileage = ''

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return mileage

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 10:
                mileage = region[u'result'][0]

        return mileage


    def __FixedMileageData__(self, mileagedata):
        return ConfidenceLevel.Bad, self.__FormatData__(self.__NormalizeData__(mileagedata))


    def __CheckData__(self, data):
        if not self.__CheckFloat__(data):
            return False

        dotindex = data.find(u'.')
        if (dotindex != -1 and (len(data) - dotindex) == 2):
            return True

        return False


    def __NormalizeData__(self, mileagedata):
        length = len(mileagedata)
        if length == 0:
            return mileagedata

        mileagedata = re.sub(u'\\$| |%|\\\\', u'', mileagedata)
        mileagedata = ''.join([ch for ch in mileagedata if (ch in self.__FloatPatterns__)])

        length = len(mileagedata)
        dotindex = mileagedata.find(u'.')
        if dotindex != -1:
            if (length - dotindex) > 2:
                return mileagedata[0: dotindex + 2]

        return mileagedata 


    def __FormatData__(self, mileagedata):
        length = len(mileagedata)
        if length == 0:
            return ''

        while len(mileagedata) > 1 and mileagedata[0] == u'0' and mileagedata[1] == u'0':
            mileagedata = mileagedata[1:]

        dotindex = mileagedata.find(u'.')
        if dotindex != -1:
            if (length - dotindex) > 2:
                return mileagedata[0: dotindex + 2]
            elif (length - dotindex) == 1:
                return mileagedata + u'0'
            return mileagedata
        else:
            return mileagedata + u'.0'