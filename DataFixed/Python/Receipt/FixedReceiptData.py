#!/usr/bin/env python2
# -*- coding:utf-8 -*-
import os
import json
import logging
import traceback
import sys
import re

from FixType import FixType

from ConfidenceLevel import ConfidenceLevel
from DateDataFixed import DateDataFixed
from Domestic.VAT.AmountDataFixed import AmountDataFixed as VAT_AmountDataFixed
from Domestic.VAT.CodeDataFixed import CodeDataFixed as VAT_CodeDataFixed
from Domestic.VAT.NumberDataFixed import NumberDataFixed as VAT_NumberDataFixed
from Domestic.VAT.CheckCodeFixed import CheckCodeFixed as VAT_CheckCodeFixed
from International.AmountDataFixed import AmountDataFixed as International_AmountDataFixed
from International.IntlDateFixed import IntlDateFixed as International_DateFixed
from International.IntlTimeFixed import IntlTimeFixed as International_TimeFixed
from Domestic.Tax.AmountDataFixed import AmountDataFixed as Tax_AmountDataFixed
from Domestic.Tax.CodeDataFixed import CodeDataFixed as Tax_CodeDataFixed
from Domestic.Tax.MileageDataFixed import MileageDataFixed as Tax_MileageDataFixed
from Domestic.Tax.NumberDataFixed import NumberDataFixed as Tax_NumberDataFixed
from Domestic.Tax.TimeDataFixed import TimeDataFixed as Tax_TimeDataFixed
from Domestic.Train.NumberDataFixed import NumberDataFixed as Train_NumberDataFixed
from Domestic.Train.AmountDataFixed import AmountDataFixed as Train_AmountDataFixed
from Domestic.Train.NumberDataFixed import NumberDataFixed as Train_NumberDataFixed
from Domestic.Train.SeatDataFixed import SeatDataFixed as Train_SeatDataFixed
from Domestic.Train.StationDataFixed import StationDataFixed as Train_StationDataFixed
from Domestic.Train.TrainNumberDataFixed import TrainNumberDataFixed as Train_TrainNumberDataFixed
from Domestic.VATPrinted.NumberDataFixed import NumberDataFixed as VATPrinted_NumberDataFixed
from Domestic.VATPrinted.CodeDataFixed import CodeDataFixed as VATPrinted_CodeDataFixed
from Domestic.VATPrinted.TimeDataFixed import TimeDataFixed as VATPrinted_TimeDataFixed
from Domestic.VATPrinted.AmountDataFixed import AmountDataFixed as VATPrinted_AmountDataFixed
from Domestic.VATRoll.SellerNameFixed import SellerNameFixed as VATRoll_SellerNameFixed
from Domestic.VATRoll.NumberDataFixed import NumberDataFixed as VATRoll_NumberDataFixed
from Domestic.VATRoll.CodeDataFixed import CodeDataFixed as VATRoll_CodeDataFixed
from Domestic.VATRoll.CheckCodeFixed import CheckCodeFixed as VATRoll_CheckCodeFixed
from Domestic.VATRoll.AmountDataFixed import AmountDataFixed as VATRoll_AmountDataFixed
from Domestic.Air.NumberDataFixed import NumberDataFixed as Air_NumberDataFixed
from Domestic.Air.CheckCodeFixed import CheckCodeFixed as Air_CheckCodeFixed
from Domestic.Air.AmountDataFixed import AmountDataFixed as Air_AmountDataFixed
from Domestic.Air.FormatReceiptData import FormatReceiptData as Air_FormatReceiptData
from Domestic.Air.NameDataFixed import NameDataFixed as Air_NameDataFixed
from Domestic.Air.IdNumberDataFixed import IdNumberDataFixed as Air_IdNumberDataFixed
from Domestic.Air.SerialNumberFixed import SerialNumberFixed as Air_SerialNumberFixed
from Domestic.Air.DateOfIssueFixed import DateOfIssueFixed as Air_DateOfIssueFixed
from Domestic.Air.InsuranceFixed import InsuranceFixed as Air_InsuranceFixed
from Domestic.Air.IssuedByFixed import IssuedByFixed as Air_IssuedByFixed
from Domestic.Quota.CodeDataFixed import CodeDataFixed as Quota_CodeDataFixed
from Domestic.Quota.NumberDataFixed import NumberDataFixed as Quota_NumberDataFixed
from Domestic.Quota.AmountInWordsFixed import AmountInWordsFixed as Quota_AmountInWordsFixed
from Domestic.VATRoll.SellerCodeFixed import SellerCodeFixed as VATRoll_SellerCodeFixed
from Domestic.VATRoll.CommodityNameFixed import CommodityNameFixed as VATRoll_CommodityNameFixed
from Domestic.Quota.WordsToNumberFixed import WordsToNumberFixed as Quota_WordsToNumberFixed
def FixedReceiptData(receiptData):
    try:

        serverType = receiptData.get(u'servertype', u'').lower()
        logging.info(u'servertype: {}'.format(serverType))

        if serverType == u'international':
            return FixedReceiptDataForInternational(receiptData)

        else:
            logging.info(serverType)
            receiptData = FixType(receiptData)
            type = receiptData.get('type', [{'label':'0'}])[0]['label']

            if type == '10500':
                return FixedReceiptDataForTax(receiptData)
            elif type == '10503':
                return FixedReceiptDataForTrain(receiptData)
            elif type == '10506':
                return FixedReceiptDataForAir(receiptData)
            elif type == '10200':
                return FixedReceiptDataForQuota(receiptData)
            elif serverType == u'domestic' or type == '10100' or type == '10101' or type == '10102' or type == '10103' or type == '10400':
                if type == '10103':
                    return FixedReceiptDataForVATRoll(receiptData)
                elif type == '10400':
                    return FixedReceiptDataForVATPrinted(receiptData)

                return FixedReceiptDataForVAT(receiptData)
        
        return receiptData
    
    except Exception as err:
        traceback.print_exc()
        logging.info(u'FixedReceiptData err: {}'.format(err))
        return receiptData
    

def FixedReceiptDataForVAT(receiptData):
    dataFixed = VAT_AmountDataFixed()
    amount_confidencelevel, before_tax, tax, after_tax = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = DateDataFixed()
    date_confidencelevel, date = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VAT_CodeDataFixed()
    code_confidencelevel, code = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VAT_NumberDataFixed()
    number_confidencelevel, number, number_order_error = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VAT_CheckCodeFixed()
    checkcode_confidencelevel, checkcode, checkcodecandidates, lastsix = dataFixed.StartFixedFromJson(receiptData)

    summaryconfidencelevel = ConfidenceLevel.Bad
    if checkcode_confidencelevel == ConfidenceLevel.Bad or date_confidencelevel == ConfidenceLevel.Bad:
        summaryconfidencelevel = ConfidenceLevel.Bad
    else:
        confidencelevellist = [amount_confidencelevel, code_confidencelevel, number_confidencelevel]
        if ConfidenceLevel.Bad in confidencelevellist:
            summaryconfidencelevel = ConfidenceLevel.Bad
        elif ConfidenceLevel.Fixed in confidencelevellist:
            summaryconfidencelevel = ConfidenceLevel.Fixed
        else:
            summaryconfidencelevel = ConfidenceLevel.Confident
        
    receiptData[u'fixedregions'] = [{u'cls':1, u'confidencelevel': code_confidencelevel.value, u'result':code}, 
                                   {u'cls':2, u'confidencelevel': number_confidencelevel.value, u'result':number}, 
                                   {u'cls':3, u'confidencelevel': date_confidencelevel.value, u'result':date}, 
                                   {u'cls':4, u'confidencelevel': amount_confidencelevel.value, u'result':before_tax}, 
                                   {u'cls':5, u'confidencelevel': checkcode_confidencelevel.value, u'result':checkcode},
                                   {u'cls':8, u'confidencelevel': amount_confidencelevel.value, u'result':tax},
                                   {u'cls':9, u'confidencelevel': amount_confidencelevel.value, u'result':after_tax}
                                   ]
    receiptData[u'extra'] = [{u'check_code_candidates':checkcodecandidates}, 
                             {u'check_code_last_six':lastsix},
                             {u'number_order_error':number_order_error}]
    receiptData[u'confidencelevel'] = summaryconfidencelevel.value

    return receiptData


def FixedReceiptDataForInternational(receiptData):
    dataFixed = International_DateFixed()
    date_confidencelevel, date = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = International_TimeFixed()
    time_confidencelevel, time = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = International_AmountDataFixed()
    confidencelevel, subtotal, tip, tip_rate, total = dataFixed.StartFixedFromJson(receiptData)
    receiptData[u'fixedregions'] = [{u'cls':3, u'result':subtotal}, 
                                   {u'cls':5, u'result':tip}, 
                                   {u'cls':6, u'result':total}, 
                                   {u'cls':11, u'result':tip_rate},
                                   {u'cls':2, u'confidencelevel': date_confidencelevel.value, u'result':date},
                                   {u'cls':7, u'confidencelevel': time_confidencelevel.value, u'result':time},
                                   ]
    receiptData[u'confidencelevel'] = confidencelevel.value
    return receiptData


def FixedReceiptDataForTax(receiptData):
    dataFixed = Tax_AmountDataFixed()
    amount_confidencelevel, amount = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Tax_CodeDataFixed()
    code_confidencelevel, code = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = DateDataFixed()
    date_confidencelevel, date = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Tax_MileageDataFixed()
    mileage_confidencelevel, mileage = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Tax_NumberDataFixed()
    number_confidencelevel, number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Tax_TimeDataFixed()
    time_confidencelevel, starttime, endtime = dataFixed.StartFixedFromJson(receiptData)

    receiptData[u'fixedregions'] = [{u'cls':1, u'confidencelevel': code_confidencelevel.value, u'result':code}, 
                                   {u'cls':2, u'confidencelevel': number_confidencelevel.value, u'result':number}, 
                                   {u'cls':3, u'confidencelevel': date_confidencelevel.value, u'result':date}, 
                                   {u'cls':9, u'confidencelevel': amount_confidencelevel.value, u'result':amount}, 
                                   {u'cls':10, u'confidencelevel': mileage_confidencelevel.value, u'result':mileage},
                                   {u'cls':31, u'confidencelevel': time_confidencelevel.value, u'result':starttime},
                                   {u'cls':32, u'confidencelevel': time_confidencelevel.value, u'result':endtime},
                                   ]
    receiptData[u'confidencelevel'] = ConfidenceLevel.Bad.value
    return receiptData


def FixedReceiptDataForTrain(receiptData):
    dataFixed = Train_AmountDataFixed()
    amount_confidencelevel, amount = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = DateDataFixed()
    date_confidencelevel, date = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Train_NumberDataFixed()
    number_confidencelevel, number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Train_SeatDataFixed()
    seat_confidencelevel, seat = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Train_StationDataFixed()
    station_confidencelevel, startstation, endstation = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Train_TrainNumberDataFixed()
    trainnumber_confidencelevel, trainnumber = dataFixed.StartFixedFromJson(receiptData)

    receiptData[u'fixedregions'] = [{u'cls':2, u'confidencelevel': number_confidencelevel.value, u'result':number}, 
                                   {u'cls':3, u'confidencelevel': date_confidencelevel.value, u'result':date}, 
                                   {u'cls':9, u'confidencelevel': amount_confidencelevel.value, u'result':amount}, 
                                   {u'cls':15, u'confidencelevel': station_confidencelevel.value, u'result':startstation},
                                   {u'cls':16, u'confidencelevel': station_confidencelevel.value, u'result':endstation},
                                   {u'cls':17, u'confidencelevel': trainnumber_confidencelevel.value, u'result':trainnumber},
                                   {u'cls':18, u'confidencelevel': seat_confidencelevel.value, u'result':seat},
                                   ]
    receiptData[u'confidencelevel'] = ConfidenceLevel.Bad.value

    return receiptData


def FixedReceiptDataForVATRoll(receiptData):
    dataFixed = DateDataFixed()
    date_confidencelevel, date = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VATRoll_NumberDataFixed()
    number_confidencelevel, number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VATRoll_CodeDataFixed()
    code_confidencelevel, code = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VATRoll_CheckCodeFixed()
    checkcode_confidencelevel, checkcode, checkcodecandidates, lastsix = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VATRoll_AmountDataFixed()
    amount_confidencelevel, total_amount = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VATRoll_SellerNameFixed()
    name_confidencelevel, name = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VATRoll_SellerCodeFixed()
    sellercode_confidencelevel, sellercode = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VATRoll_CommodityNameFixed()
    commodityname_confidencelevel, commodityname = dataFixed.StartFixedFromJson(receiptData)

    summaryconfidencelevel = ConfidenceLevel.Bad
    if number_confidencelevel == ConfidenceLevel.Bad or checkcode_confidencelevel == ConfidenceLevel.Bad or date_confidencelevel == ConfidenceLevel.Bad:
        summaryconfidencelevel = ConfidenceLevel.Bad
    else:
        confidencelevellist = [code_confidencelevel, number_confidencelevel]
        if ConfidenceLevel.Bad in confidencelevellist:
            summaryconfidencelevel = ConfidenceLevel.Bad
        elif ConfidenceLevel.Fixed in confidencelevellist:
            summaryconfidencelevel = ConfidenceLevel.Fixed
        else:
            summaryconfidencelevel = ConfidenceLevel.Confident

    receiptData[u'fixedregions'] = [{u'cls':1, u'confidencelevel': code_confidencelevel.value, u'result':code}, 
                                {u'cls':2, u'confidencelevel': number_confidencelevel.value, u'result':number}, 
                                {u'cls':3, u'confidencelevel': date_confidencelevel.value, u'result':date}, 
                                {u'cls':5, u'confidencelevel': checkcode_confidencelevel.value, u'result':checkcode},
                                {u'cls':9, u'confidencelevel': amount_confidencelevel.value, u'result': total_amount},
                                {u'cls':27, u'confidencelevel': name_confidencelevel.value, u'result': name},
                                {u'cls':28, u'confidencelevel': sellercode_confidencelevel.value, u'result': sellercode},
                                {u'cls':20, u'confidencelevel': commodityname_confidencelevel.value, u'result': commodityname}
                                ]
    receiptData[u'extra'] = [{u'check_code_candidates':checkcodecandidates}, 
                             {u'check_code_last_six':lastsix},
                             ]
    receiptData[u'confidencelevel'] = summaryconfidencelevel.value

    return receiptData
    

def FixedReceiptDataForVATPrinted(receiptData):
    dataFixed = DateDataFixed()
    date_confidencelevel, date = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VATPrinted_NumberDataFixed()
    number_confidencelevel, number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VATPrinted_CodeDataFixed()
    code_confidencelevel, code = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VATPrinted_TimeDataFixed()
    time_confidencelevel, time = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = VATPrinted_AmountDataFixed()
    amount_confidencelevel, total_amount = dataFixed.StartFixedFromJson(receiptData)



    if number_confidencelevel == ConfidenceLevel.Bad or date_confidencelevel == ConfidenceLevel.Bad or code_confidencelevel == ConfidenceLevel.Bad:
        summaryconfidencelevel = ConfidenceLevel.Bad
    else:
        confidencelevellist = [code_confidencelevel, number_confidencelevel]
        if ConfidenceLevel.Bad in confidencelevellist:
            summaryconfidencelevel = ConfidenceLevel.Bad
        elif ConfidenceLevel.Fixed in confidencelevellist:
            summaryconfidencelevel = ConfidenceLevel.Fixed
        else:
            summaryconfidencelevel = ConfidenceLevel.Confident

    receiptData[u'fixedregions'] = [{u'cls':1, u'confidencelevel': code_confidencelevel.value, u'result':code}, 
                                {u'cls':2, u'confidencelevel': number_confidencelevel.value, u'result':number}, 
                                {u'cls':3, u'confidencelevel': date_confidencelevel.value, u'result':date},
                                {u'cls':19, u'confidencelevel': time_confidencelevel.value, u'result':time},
                                {u'cls':9, u'confidencelevel': amount_confidencelevel.value, u'result': total_amount},
                                ]
    receiptData[u'confidencelevel'] = summaryconfidencelevel.value

    return receiptData


def FixedReceiptDataForAir(receiptData):
    dataFixed = Air_NameDataFixed()
    name_confidencelevel, name = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Air_IdNumberDataFixed()
    id_number_confidencelevel, id_number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Air_NumberDataFixed()
    number_confidencelevel, number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Air_CheckCodeFixed()
    checkcode_confidencelevel, checkcode, checkcodecandidates = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Air_AmountDataFixed()
    amount_confidencelevel, amount = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Air_SerialNumberFixed()
    serial_number_confidence, serial_number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Air_DateOfIssueFixed()
    date_of_issue_confidence, date_of_issue = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Air_InsuranceFixed()
    insurance_confidence, insurance = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Air_IssuedByFixed()
    issued_by_confidence, issued_by = dataFixed.StartFixedFromJson(receiptData)

    receiptData, fixedregions = Air_FormatReceiptData(receiptData).Format()

    receiptData[u'fixedregions'] = [{u'cls':2, u'confidencelevel': number_confidencelevel.value, u'result':number}, 
                                {u'cls':5, u'confidencelevel': checkcode_confidencelevel.value, u'result':checkcode},
                                {u'cls':21, u'confidencelevel': amount_confidencelevel.value, u'result':amount},
                                {u'cls':22, u'confidencelevel': id_number_confidencelevel.value, u'result':id_number},
                                {u'cls':23, u'confidencelevel': name_confidencelevel.value, u'result':name},
                                {u'cls':38, u'confidencelevel': serial_number_confidence.value, u'result':serial_number},
                                {u'cls':40, u'confidencelevel': issued_by_confidence.value, u'result':issued_by},
                                {u'cls':41, u'confidencelevel': date_of_issue_confidence.value, u'result':date_of_issue},
                                {u'cls':42, u'confidencelevel': insurance_confidence.value, u'result':insurance},
                                ]

    for region in fixedregions:
        receiptData[u'fixedregions'].append({
            u'cls': region[u'cls'],
            u'confidencelevel': region[u'fixed_confidence'],
            u'result': region[u'fixed'],
        })

    receiptData[u'extra'] = [{u'check_code_candidates':checkcodecandidates}, 
                            ]

    return receiptData


def FixedReceiptDataForQuota(receiptData):
    dataFixed = Quota_CodeDataFixed()
    code_confidencelevel, code = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Quota_NumberDataFixed()
    number_confidencelevel, number = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Quota_AmountInWordsFixed()
    amount_confidencelevel, amount = dataFixed.StartFixedFromJson(receiptData)

    dataFixed = Quota_WordsToNumberFixed(receiptData)
    amount_number_confidencelevel, amount_number = dataFixed.StartFixedFromJson(receiptData)

    receiptData[u'fixedregions'] = [{u'cls':1, u'confidencelevel': code_confidencelevel.value, u'result': code},
                                {u'cls':2, u'confidencelevel': number_confidencelevel.value, u'result': number},
                                {u'cls':29, u'confidencelevel': amount_confidencelevel.value, u'result': amount},
                                ]

    return receiptData


if __name__ == '__main__':
    logging.basicConfig(stream=sys.stdout,)
    logging.getLogger().setLevel(logging.INFO)
    ch = logging.StreamHandler()
    ch.setLevel(logging.INFO)
    formatter = logging.Formatter(
        "%(asctime)s;%(levelname)s;%(message)s", "%Y-%m-%d %H:%M:%S")
    ch.setFormatter(formatter)
    logging.getLogger().addHandler(ch)

    houselist = re.findall(u'[日| ](\\d*):', '2017年02月23日 20:57')
    #dataFixed = VAT_AmountDataFixed()
    #dataFixed = VAT_DateDataFixed()
    #dataFixed = VAT_CodeDataFixed()
    #dataFixed = VAT_NumberDataFixed()
    #dataFixed = VAT_CheckCodeFixed()
    #dataFixed.StartFixedFromPath(u'C:/Users/User/Desktop/receipt/Domestic/result/', u'C:/Users/User/Desktop/receipt/Domestic/validated/')
    #dataFixed = International_AmountDataFixed()
    #dataFixed.StartFixedFromPath(u'C:/Users/User/Desktop/receipt/International/result/', 'C:/Users/User/Desktop/receipt/International/validated/')
    #dataFixed = Tax_DateDataFixed()
    #dataFixed.StartFixedFromPath(u'C:/Users/User/Desktop/receipt/Tax/result/', 'C:/Users/User/Desktop/receipt/Tax/validated/')
    # dataFixed = Train_SeatDataFixed()
    # dataFixed.StartFixedFromPath(u'C:/Users/User/Desktop/receipt/Train/result/', 'C:/Users/User/Desktop/receipt/Train/validated/')
    
    