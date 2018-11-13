# -*- coding: utf-8 -*-

import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class NameDataFixed(DataFixed):

    def __init__(self):
        DataFixed.__init__(self, 'CommodityName')

    def __FixedData__(self, resultJson):
        commodity_names = self.__ParseData__(resultJson)
        if len(commodity_names) == 0:
            logging.info(u'CommodityName Name Error')
            return ConfidenceLevel.Bad, ''

        logging.info(commodity_names[0] + u' Fixed To ')


    #首先分析JSON
    def __ParseData__(self, jsondata):
        commodity_names = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return commodity_names

        regions = jsondata[u'regions']
        regions = sorted(regions, key=lambda region: region[u'confidence'], reverse = True)

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 20:
                for result in region[u'result']:
                    if len(result):
                        commodity_names.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        commodity_names.append(result)

        return commodity_names

    def __FixedCommodityNames__(self, commodity_names):

        return ConfidenceLevel.Bad, ''






