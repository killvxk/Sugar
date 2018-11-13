# -*- coding: utf-8 -*-

import logging
import re
import Domestic.VATRoll.CompanyList as CompanyList
from utils import StrUtil
from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class SellerNameFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'SellerName')

    
    def __FixedData__(self, resultJson):
        seller_names = self.__ParseData__(resultJson)
        if len(seller_names) == 0:
            logging.info(u'Seller Name Error')
            return ConfidenceLevel.Bad, ''

        logging.info(seller_names[0] + u' Fixed To ')
        
        seller_names = self.__ReplaceSpecialChar__(seller_names)
        seller_names = self.__ReplaceSonCompany__(seller_names)
        confidencelevel, name = self.__FixedSellerNames__(seller_names)

        logging.info(name)
        logging.info(confidencelevel)
        return confidencelevel, name


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        seller_names = self.__ParseData__(resultJson)
        validated_seller_names = self.__ParseData__(validateJson)

        if len(validated_seller_names) == 0 or len(seller_names) == 0 or not self.__CheckData__(validated_seller_names[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_seller_names[0] != seller_names[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(seller_names[0] + u' Fixed To ')
        
        confidencelevel, name = self.__FixedSellerNames__(seller_names)

        logging.info(name)
        logging.info(confidencelevel)

        if validated_seller_names[0] == name:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_seller_names[0])
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        seller_names = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return seller_names

        regions = jsondata[u'regions']
        regions = sorted(regions, key=lambda region: region[u'confidence'], reverse = True)

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 27:
                for result in region[u'result']:
                    if len(result):
                        seller_names.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        seller_names.append(result)

        return seller_names


    def __CheckData__(self, data):
        return True

    #对比和相似度比较阶段
    def __FixedSellerNames__(self, seller_names):
        min_similarity = 1000
        result = ''

        for CompanyName in seller_names:
            if CompanyName in CompanyList.CompanyList:
                return ConfidenceLevel.Confident, CompanyName

            for CompanyListName in CompanyList.CompanyList:
                similarity = StrUtil.Similarity(CompanyListName, CompanyName)
                if similarity < len(CompanyName) and min_similarity >= similarity:
                    if (min_similarity == similarity and len(result) >= len(CompanyListName)):
                        continue

                    min_similarity = similarity
                    result = CompanyListName

            if min_similarity == 1000:
                return ConfidenceLevel.Bad, CompanyName
            else:
                return ConfidenceLevel.Fixed, result

        return ConfidenceLevel.Bad, ''

    def __FixSellerName__(self, seller_names):
        return ConfidenceLevel.Bad, seller_names[0]

    #修复部分公司名称的特殊标点和数字字母
    def __ReplaceSpecialChar__(self, seller_names):
        replaced_names = []
        for index in range(len(seller_names)):
            name = re.sub(r'\\|n', u'', seller_names[index])
            name = re.sub(u'[a-z]|[A-Z]|[0-9]', u'', name)
            replaced_names.append(name)

        return replaced_names

    #修复分公司和壳牌的问题
    def __ReplaceSonCompany__(self, seller_names):
        replaced_names = []
        for index in range(len(seller_names)):
            name = re.sub(u'公+司+', u'公司', seller_names[index])
            name = re.sub(u'分+公*司+|分+司|分+公+', u'分公司', name)
            name = re.sub(u'亮牌', u'壳牌', name)
            name = re.sub(u'[a-z]|[A-Z]|[0-9]', u'', name)
            replaced_names.append(name)

        return replaced_names