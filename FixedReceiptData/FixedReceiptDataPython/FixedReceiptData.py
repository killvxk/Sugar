#!/usr/bin/env python3
# -*- coding:utf-8 -*-

from Domestic.AmountDataFixed import AmountDataFixed

if __name__ == '__main__':
    dataFixed = AmountDataFixed('C:/Users/User/Desktop/receipt/Domestic/validated/', 'C:/Users/User/Desktop/receipt/Domestic/result/')
    dataFixed.StartFixed()
