# -*- encoding: utf-8 -*-

import logging
import re

from ConfidenceLevel import ConfidenceLevel
from DataFixed import DataFixed

class CostFixed(DataFixed):
    def __init__(self):
        DataFixed.__init__(self, u'Cost')
        self.__Patterns__ = r'([0-9.]*?)(CNY|CN|AE|DC|XT|YQ){0, 1}'


    def __ParseData__(self, resultJson):
        cost_list = []

        if resultJson == None or not isinstance(resultJson, dict):
            return cost_list
    
        for result in resultJson.get(u'result', []):
            if len(result):
                cost_list.append(result)

        for result in resultJson.get(u'ref_result', []):
            if len(result):
                cost_list.append(result)

        result = []
        for cost in cost_list:
            result.append(re.sub(r'[^A-Z0-9\. ]', u'', cost))
    
        return result


    def __FixedData__(self, resultJson):
        cost_list = self.__ParseData__(resultJson)
        if not len(cost_list):
            logging.info(u'Validated Data Error')
            return

        logging.info(cost_list[0] + u' Fixed To ')
        cost_confidence, cost = self.__FixCost__(cost_list)

        logging.info(cost)

        return cost_confidence, cost

    def __FixCost__(self, cost_list):
        for cost in cost_list:
            regex = re.search(self.__Patterns__, cost.upper())
            if regex:
                return ConfidenceLevel.Fixed, cost.upper()

        return ConfidenceLevel.Bad, cost_list[0]
        