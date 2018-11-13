# -*- coding: utf-8 -*-

import logging
import re
from utils import StrUtil
from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel


class CommodityNameFixed(DataFixed):
    def __init__(self):
        DataFixed.__init__(self, "CommodityName")
        self.__Suffix__ = (u'90#', u'92#', u'93#', u'95#', u'97#',u'0#')
        self.__type__ = {1: u'oil'}

    def __FixedData__(self, resultJson):
        commodity_list = self.__ParseData__(resultJson)
        if len(commodity_list) == 0:
            logging.info(u'Commodity Name Error')
            return ConfidenceLevel.Bad, ''

        logging.info(commodity_list[0] + u' Fixed To ')
        TypedCommodity = self.__MatchType__(commodity_list)
        confidencelevel, commodity_name = self.__GiveType__(TypedCommodity)

        logging.info(commodity_name)

        return confidencelevel, commodity_name


    #预处理整个json，提取出品名的数组 ；return 数组
    def __ParseData__(self, jsondata):
        commodity_names = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return commodity_names

        regions = jsondata[u'regions']
        regions = sorted(regions, key=lambda region: region[u'confidence'], reverse=True)

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

    #根据品名进行正则判断卷票类别
    def __MatchType__(self, commodity_Names):
        #logging.info(commodity_Names)
        fixed_name = []
        if not len(commodity_Names):
            return fixed_name
        for index in range(len(commodity_Names)):
            resulta = re.search(u'(汽|油|车).*油', commodity_Names[index])
            resultb = re.search(u'(90|92|93|95|97|0)(#|号)*.*油', commodity_Names[index])
            resultx = commodity_Names[index] in self.__Suffix__
            if resulta or resultb or resultx:
                fixed_name.append(self.__type__.get(1))
        return fixed_name
        #return fixed_name

    def __GiveType__(self, TypedCommodity):
        if not len(TypedCommodity):
            return ConfidenceLevel.Bad, ''
        return ConfidenceLevel.Fixed, TypedCommodity[0]

