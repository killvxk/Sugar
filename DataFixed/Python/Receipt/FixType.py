# -*- coding: utf-8 -*-
import logging

from Domestic.Tax.TimeDataFixed import TimeDataFixed as Tax_TimeDataFixed
from Domestic.Tax.MileageDataFixed import MileageDataFixed as Tax_MileageDataFixed
from Domestic.VAT.CheckCodeFixed import CheckCodeFixed as VAT_CheckCodeFixed

def FixType(receiptData):
    if not receiptData.get('type'):
        receiptData[u'type'] = [{'label':'0'}]

    type = receiptData.get('type')[0]['label']
    logging.info(type)

    if (type == '10103' or type == '10400' or type == '10500'):
        dataFixed = Tax_TimeDataFixed()
        starttime, endtime = dataFixed.__ParseData__(receiptData)

        dataFixed = Tax_MileageDataFixed()
        mileage = dataFixed.__ParseData__(receiptData)

        if len(starttime) or len(endtime) or len(mileage):
            receiptData[u'type'][0][u'label'] = '10500'
        elif type == '10500':
            receiptData[u'type'][0][u'label'] = '10500'

    if (type == '10100' or type == '10101'):
        dataFixed = VAT_CheckCodeFixed()
        check_code_list = dataFixed.__ParseData__(receiptData)
        logging.info(u'fix VAT: check_code_list: {}'.format(check_code_list))
        if len(check_code_list):
            receiptData[u'type'][0][u'label'] = '10101'
        else:
            receiptData[u'type'][0][u'label'] = '10100'

    logging.info(u'fix type {} to {}'.format(type, receiptData[u'type'][0][u'label']))

    return receiptData