# -*- encoding: utf-8 -*-

import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class IdentifyAmount(DataFixed):
    def __init__(self):
        DataFixed.__init__(self, u'Identify')
        self.__KeyPatterns__ = r'YQ|XT|CN'
        self.__DefaultKeys__ = [u'basic_fare', u'caac_development_fund', u'fuel_surcharge', u'tax', u'after_tax_amount']
        self.__Patterns__ = r'([A-Z]*)([0-9.]*)([A-Z]*)([0-9.]*)([A-Z]*)([0-9.]*)([A-Z]*)([0-9.]*)([A-Z]*)([0-9.]*)'

    def __ParseData__(self, resultJson):
        amount_list = []

        if resultJson == None or not isinstance(resultJson, dict):
            return amount_list
    
        for result in resultJson.get(u'result', []):
            if len(result):
                amount_list.append(result)

        for result in resultJson.get(u'ref_result', []):
            if len(result):
                amount_list.append(result)

        result = []
        for amount_str in amount_list:
            result.append(re.sub(r'[^A-Z0-9\.]', u'', amount_str.upper()))
    
        return result

    def __FixedData__(self, resultJson):
        amount_list = self.__ParseData__(resultJson)
        if not len(amount_list):
            logging.info(u'Validated Data Error')
            return

        logging.info(amount_list[0] + u' Fixed To ')
        cost_confidence, fixedcost = self.__FixCost__(amount_list)

        logging.info(fixedcost)

        return cost_confidence, fixedcost

    def __FixCost__(self, amount_list):
        for cost in amount_list:
            regex = re.search(self.__Patterns__, cost)
            if regex:
                keys = []
                values = []
                for index in range(5):
                    if len(regex.group(index * 2 + 1)):
                        keys.append(regex.group(index * 2 + 1))
                        values.append(regex.group((index + 1) * 2))

                if len(keys) <= 1:
                    continue

                try:
                    total = 0
                    for index in range(len(keys) - 1):
                        total += float(values[index])

                    if int(total) != int(float(values[-1])):
                        continue
                except Exception:
                    continue

                keys[0] = self.__DefaultKeys__[0]
                keys[-1] = self.__DefaultKeys__[-1]
                fixedcost = {}
                for index in range(len(keys)):
                    if 0 < index < len(keys) - 1:
                        keys[index] = self.__DefaultKeys__[index]

                    fixedcost[keys[index]] = self.__FixMoney__(values[index])

                return ConfidenceLevel.Confident, fixedcost

        return ConfidenceLevel.Bad, None

    def __FixMoney__(self, money):
        regex = re.search(r'([1-9]\d*\.00)', money)
        if regex:
            return regex.group(1)

        temp_money = u''
        for ch in re.sub(r'[^0-9\.]', u'', money):
            if not temp_money and ch != u'0':
                temp_money += ch

            else:
                temp_money += ch

        return temp_money
        