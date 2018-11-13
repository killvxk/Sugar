import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel


class InsuranceFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'Amount')
        self.__Default__ = u'XXX'


    def __FixedData__(self, resultJson):
        insurance_list = self.__ParseData__(resultJson)

        if len(insurance_list) == 0:
            logging.info(u'Date Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(insurance_list[0] + u' Fixed To ')

        confidenceLevel, insurance = self.__FixedInsurance__(insurance_list)

        logging.info(insurance)

        return confidenceLevel, insurance


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_insurance_list = self.__ParseData__(resultJson)
        validate_insurance_list = self.__ParseData__(validateJson)

        if len(validate_insurance_list) == 0 or len(result_insurance_list) == 0 or not self.__CheckData__(validate_insurance_list[0]):
            logging.info(u'Validated Data Error')
            return

        if validate_insurance_list[0] != result_insurance_list[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_insurance_list[0] + u' Fixed To ')

        confidenceLevel, insurance = self.__FixedInsurance__(result_insurance_list)

        logging.info(insurance)

        if validate_insurance_list[0] == insurance:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validate_insurance_list[0])
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        insurance = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return insurance

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 42:
                for result in region[u'result']:
                    if len(result):
                        insurance.append(result)

                if region.get(u'ref_result') != None:
                    for result in region[u'ref_result']:
                        if len(result):
                            insurance.append(result)

        result = []
        for ins in insurance:
            result.append(re.sub(r'[^A-Z0-9\.]', u'', ins.upper()))

        return result


    def __CheckData__(self, data):
        if data == self.__Default__:
            return True

        if not self.__CheckNumber__(data):
            return False

        return False


    def __FixedInsurance__(self, insurancelist):
        for insurance in insurancelist:
            if self.__CheckNumber__(insurance) or self.__CheckFloat__(insurance):
                return ConfidenceLevel.Fixed, insurance

        regex = re.search(r'[A-Z]', insurancelist[0].upper())
        if regex:
            return ConfidenceLevel.Fixed, self.__Default__

        return ConfidenceLevel.Bad, insurancelist[0].strip()
