# -*- coding: utf-8 -*-

import FixedReceiptData
import json

if __name__ == '__main__':
    
    receiptData = json.load(open(u'./data/11111.json'))
    receiptData = FixedReceiptData.FixedReceiptData(receiptData)
    print receiptData
    json.dump(receiptData,  open(u'./data/11111-2.json', "w"), indent = 4)
    #file.close()
    pass