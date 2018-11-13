# -*- encoding: utf-8 -*-

import logging
import re

from ConfidenceLevel import ConfidenceLevel
from DateDataFixed import DateDataFixed

class DateOfIssueFixed(DateDataFixed):
    """description of class"""

    def __init__(self):
        DateDataFixed.__init__(self)

    def __ParseData__(self, jsondata):
        datelist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return datelist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 41:
                for result in region[u'result']:
                    if len(result):
                        datelist.append(result)

                if region.get(u'ref_result') != None:
                    for result in region[u'ref_result']:
                        if len(result):
                            datelist.append(result)

        # trip 开头不属于日期的字母
        flitered_datelist = []
        for origin_date in datelist:
            flitered_datelist.append(re.sub(r'^[^0-9\-]*', u'', origin_date).strip())

        return flitered_datelist
