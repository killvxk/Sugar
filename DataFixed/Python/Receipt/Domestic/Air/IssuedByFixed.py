# -*- encoding: utf-8 -*-

import logging
import re

from ConfidenceLevel import ConfidenceLevel
from DataFixed import DataFixed
from CompanyOfIssuedBy import CompanyOfUssuedBy as candidate_companies
from utils import StrUtil

class IssuedByFixed(DataFixed):
    def __init__(self):
        DataFixed.__init__(self, u'Issued By')
        self.__Threshold__ = 0.76

    def __FixedData__(self, resultJson):
        companies = self.__ParseData__(resultJson)
        if len(companies) == 0:
            logging.info(u'Issued By Error')
            return ConfidenceLevel.Bad, ''

        logging.info(companies[0] + u' Fixed To ')
        
        confidencelevel, company = self.__FixedName__(companies)

        logging.info(company)

        return confidencelevel, company


    def __FixedDataWithValidate__(self, resultJson, validateJson):  
        result_issued_by = self.__ParseData__(resultJson)
        validated_issued_by = self.__ParseData__(validateJson)

        if len(validated_issued_by) == 0 or len(result_issued_by) == 0 or not self.__CheckData__(validated_issued_by[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_issued_by[0] != result_issued_by[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_issued_by[0] + u' Fixed To ')
        
        
        confidencelevel, number = self.__FixedName__(result_issued_by)

        logging.info(number)

        if validated_issued_by[0] == number:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_issued_by[0])
            logging.info(u'Fixed Falied!')
        

    def __ParseData__(self, jsondata):
        companies = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions0'] == None:
            return companies

        regions = jsondata[u'regions0']
        regions = sorted(regions, key=lambda region: region[u'confidence'], reverse = True)

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None or region[u'confidence'] == None:
                continue

            cls = region[u'cls']
            if cls == 40:
                for result in region[u'result']:
                    if len(result):
                        companies.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        companies.append(result)

        return companies

    def __FixedName__(self, names):
        for name in names:
            if name in candidate_companies:
                return ConfidenceLevel.Confident, name

        for name in names:
            simi_name = self.__GetSimilarityName__(name)
            if simi_name:
                return ConfidenceLevel.Fixed, simi_name

        return ConfidenceLevel.Bad, names[0]

    def __CheckData__(self, name):
        if not len(name):
            return False

        return True

    def __GetSimilarityName__(self, name):
        if not len(name):
            return

        max_count = 0
        target = name
        name = re.sub(u'责|任|航|空|服|务|有|限|公|司', u'', name)
        for candidate in candidate_companies:
            diff_count = StrUtil.Similarity(candidate, name)
            if max_count < len(candidate) - diff_count:
                max_count = len(candidate) - diff_count
                target = candidate

        if float(max_count) / len(name) > self.__Threshold__:
            return target
        
        return None

