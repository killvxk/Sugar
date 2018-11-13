# -*- encoding: utf-8 -*-

import logging
import math
import re

from StationFixed import StationFixed
from ConfidenceLevel import ConfidenceLevel
from FlightNumberFixed import FlightNumberFixed
from FlightDateDataFixed import FlightDateDataFixed
from SeatLevelFixed import SeatLevelFixed
from TimeDataFixed import TimeDataFixed
from CostFixed import CostFixed
from AgentCodeFixed import AgentCodeFixed
from IdentifyAmount import IdentifyAmount

class FormatReceiptData(object):
    """description of class"""
    def __init__(self, resultJson):
        self.data = resultJson
        self.__MinSlope__ = 5
        self.__StationGap__ = 40
        self.__Patterns__ = r'([A-Z]*)([0-9.]*)([A-Z]*)([0-9.]*)([A-Z]*)([0-9.]*)([A-Z]*)([0-9.]*)([A-Z]*)([0-9.]*)'
        self.__CostKeysPatterns__ = r'(CNY|AE|DC|XT|YQ|CN)'
        self.__KeyMapping__ = {
            u'basic_fare': [u'fare'],
            u'caac_development_fund': [u'CN', u'IN', u'AE', u'IL'],
            u'fuel_surcharge': [u'YQ', u'WO'],
            u'tax': [u'XT'],
            u'after_tax_amount': [u'total']
        }

    def Format(self):
        station_cls = self.__ParseData__(24)
        flight_cls = self.__ParseData__(17)
        date_cls = self.__ParseData__(3)
        seatlevels_cls = self.__ParseData__(18)
        time_cls = self.__ParseData__(19)

        flight_count = max(len(date_cls), len(flight_cls))

        for station_item in station_cls:
            station_confidence_level, station = StationFixed().StartFixedFromJson(station_item)
            station_item[u'fixed'] = station
            station_item[u'fixed_confidence'] = station_confidence_level.value

        for flight in flight_cls:
            flight_number_confidence_level, flight_number = FlightNumberFixed().StartFixedFromJson(flight)
            flight[u'fixed'] = flight_number
            flight[u'fixed_confidence'] = flight_number_confidence_level.value

        for date_dict in date_cls:
            date_confidence_level, date = FlightDateDataFixed().StartFixedFromJson(date_dict)
            date_dict[u'fixed'] = date.strip()
            date_dict[u'fixed_confidence'] = date_confidence_level.value

        for seat_level_item in seatlevels_cls:
            seat_confidence_level, seat_level = SeatLevelFixed().StartFixedFromJson(seat_level_item)
            seat_level_item[u'fixed'] = seat_level.strip()
            seat_level_item[u'fixed_confidence'] = seat_confidence_level.value

        for time_item in time_cls:
            time_confidence_level, time = TimeDataFixed().StartFixedFromJson(time_item)
            time_item[u'fixed'] = time.strip()
            time_item[u'fixed_confidence'] = time_confidence_level.value

        station_cls = sorted(station_cls, key = lambda station: station[u'region_3point'][1])
        flight_cls = sorted(flight_cls, key = lambda flight: flight[u'region_3point'][1])
        date_cls = sorted(date_cls, key = lambda date: date[u'region_3point'][1])
        seatlevels_cls = sorted(seatlevels_cls, key = lambda seatlevel: seatlevel[u'region_3point'][1])
        time_cls = sorted(time_cls, key = lambda time: time[u'region_3point'][1])

        # 个数相同, station, flight_number 和 date 一一对应
        if len(station_cls) - 1 == len(flight_cls) == len(date_cls) == len(seatlevels_cls) == len(time_cls) and len(flight_cls):

            flights = []
            for index in range(flight_count):
                flights.append({
                    u'from': station_cls[index][u'fixed'],
                    u'to': station_cls[index + 1][u'fixed'],
                    u'train_number': flight_cls[index][u'fixed'],
                    u'seat_level': seatlevels_cls[index][u'fixed'],
                    u'date': date_cls[index][u'fixed'],
                    u'time': time_cls[index][u'fixed'],
                })

            self.data[u'fixedflights'] = flights

        # 个数不同, 需要根据高度判断
        else:
            flight_and_date = self.__FormatTwoDataInARow__(flight_cls, date_cls, u'flight', u'date', self.__MinSlope__)
            flight_date_seat = self.__FormatTwoDataInARow__(flight_and_date, seatlevels_cls, u'flight_date', u'seat', self.__MinSlope__)
            flight_date_seat_time = self.__FormatTwoDataInARow__(flight_date_seat, time_cls, u'flight_date_seat', u'time', self.__MinSlope__)
            station_flight_date_seat_time = self.__BindStationAndOthers__(station_cls, flight_date_seat_time, u'station', u'flight_date_seat_time')

            flights = []

            for index in range(len(station_flight_date_seat_time) - 1):
                flight = {}
                from_station = station_flight_date_seat_time[index].get(u'station')
                if from_station:
                    flight[u'from'] = from_station[u'fixed']
                
                if index + 1 < len(station_flight_date_seat_time):
                    to_station = station_flight_date_seat_time[index + 1].get(u'station')
                    if to_station:
                        flight[u'to'] = to_station[u'fixed']

                result_flight_date_seat_time = station_flight_date_seat_time[index].get(u'flight_date_seat_time')
                if result_flight_date_seat_time:
                    origin_time = result_flight_date_seat_time.get(u'time')
                    if origin_time:
                        flight[u'time'] = origin_time[u'fixed']

                    result_flight_date_seat = result_flight_date_seat_time.get(u'flight_date_seat')
                    if result_flight_date_seat:
                        origin_seat = result_flight_date_seat.get(u'seat')
                        if origin_seat:
                            flight[u'seat_level'] = origin_seat[u'fixed']

                        result_flight_date = result_flight_date_seat.get(u'flight_date')
                        if result_flight_date:
                            origin_flight = result_flight_date.get(u'flight')
                            if origin_flight:
                                flight[u'train_number'] = origin_flight[u'fixed']

                            origin_date = result_flight_date.get(u'date')
                            if origin_date:
                                flight[u'date'] = origin_date[u'fixed']
                
                flights.append(flight)

            self.data[u'fixedflights'] = flights

        amount_cls = self.__ParseData__(21)
        fixedcost = None
        if len(amount_cls):
            _, fixedcost = IdentifyAmount().StartFixedFromJson(amount_cls[0])

        if not fixedcost:
            logging.info(u'cost used 44')
            _, fixedcost = self.__FixCost__()

        self.data[u'fixedcost'] = fixedcost

        agent_codes = self.__ParseData__(39)
        agent_codes = sorted(agent_codes, key = lambda code: code[u'region_3point'][1])
        for code_item in agent_codes:
            code_confidence_level, code = AgentCodeFixed().StartFixedFromJson(code_item)
            code_item[u'fixed'] = code.strip()
            code_item[u'fixed_confidence'] = code_confidence_level.value

        fixed_agent_code = u''
        for index in range(len(agent_codes)):
            agent_code_seperator = u','
            if index == 0:
                agent_code_seperator = u''

            fixed_agent_code += agent_code_seperator + agent_codes[index][u'fixed']

        self.data[u'fixed_agent_code'] = fixed_agent_code

        return self.data, station_cls + flight_cls + date_cls + seatlevels_cls + time_cls + agent_codes


    def __ParseData__(self, cls_num):
        cls_itmes = []

        if self.data == None or not isinstance(self.data, dict) or self.data[u'regions0'] == None:
            return cls_itmes

        regions = self.data[u'regions0']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None or region[u'confidence'] == None:
                continue

            cls = region[u'cls']
            if cls == cls_num:
                cls_itmes.append(region)

        return cls_itmes


    def __FormatTwoDataInARow__(self, first_data, second_data, first_key, second_key, default_min_slop = float('inf')):
        if len(first_data) <= len(second_data):
            firstlist = first_data
            secondlist = second_data
            tag = 1
        else:
            firstlist = second_data
            secondlist = first_data
            tag = -1

        if len(firstlist) == 0:
            result = []
            secondlist = sorted(secondlist, key = lambda date: date[u'region_3point'][1])
            for second_item in secondlist:
                result_item = {
                    u'region_3point': second_item[u'region_3point'],
                    u'rotation': second_item[u'rotation'],
                }
                if tag == 1:
                    result_item[second_key] = second_item
                else:
                    result_item[first_key] = second_item
                result.append(result_item)

            return result

        status = [False] * max(len(first_data), len(second_data))

        result = []
        coordinates_x = 0
        for first_item in firstlist:
            target_index = -1
            target = None
            min_slope = float('inf')
            first_point = { u'x': first_item[u'region_3point'][0], u'y': first_item[u'region_3point'][1] }
            for index in range(len(secondlist)):
                second_item = secondlist[index]
                second_point = { u'x': second_item[u'region_3point'][0], u'y': second_item[u'region_3point'][1] }
                second_center_y = (second_item[u'region_3point'][1] + second_item[u'region_3point'][5]) / 2
                slope = tag * self.__CalculateSlope__(first_point, second_point)
                diff_slope = abs(slope - math.tan(first_item[u'rotation'] / 360))
                if not status[index] and (min_slope > diff_slope and diff_slope < default_min_slop) or ( first_item[u'region_3point'][1] <= second_center_y <= first_item[u'region_3point'][5]):
                    target_index = index
                    target = second_item
                    min_slope = diff_slope

            coordinates_x = first_item[u'region_3point'][0]
            result_item = {
                    u'region_3point': first_item[u'region_3point'],
                    u'rotation': first_item[u'rotation'],
            }

            if tag == 1:
                result_item[first_key] = first_item
                result_item[second_key] = target
            else:
                result_item[first_key] = target
                result_item[second_key] = first_item

            result.append(result_item)
            if target_index != -1:
                status[target_index] = True
        
        for index in range(len(status)):
            if not status[index]:
                result_item = {
                    u'region_3point': secondlist[index][u'region_3point'],
                    u'rotation': secondlist[index][u'rotation'],
                }
                result_item[u'region_3point'][0] = coordinates_x

                if tag == 1:
                    result_item[second_key] = secondlist[index]
                else:
                    result_item[first_key] = secondlist[index]
                
                result.insert(index, result_item)

        return result


    def __CalculateSlope__(self, point_one, point_two):
        return (point_two[u'y'] - point_one[u'y']) / (point_two[u'x'] - point_one[u'x'])


    def __BindStationAndOthers__(self, stations, others, key_station, key_others):
        result = []

        if len(others) == len(stations) - 1:
            index = 0
            for other in others:
                item = {}
                item[key_station] = stations[index]
                item[key_others] = other
                result.append(item)
                index += 1

            item = {}
            item[key_station] = stations[index]
            result.append(item)

        else:
            have_use_station = False
            status = [False] * len(stations)
            for other in others:
                index = 0
                item = {}
                item[key_others] = other
                for station in stations:
                    if not status[index] and station[u'region_3point'][1] <= other[u'region_3point'][1] <= station[u'region_3point'][5] + self.__StationGap__:
                        item[key_station] = station
                        status[index] = True
                        have_use_station = True
                        break

                    index += 1

                result.append(item)

            for index in range(len(status)):
                if not status[index]:
                    item = {}
                    item[key_station] = stations[index]
                    if have_use_station:
                        result.insert(index, item)
                    else:
                        result.append(item)

            if len(result) == len(others):
                item = {}
                item[key_station] = { u'fixed': '' }
                result.append(item)

        return result


    def __FixCost__(self):
        cost_cls = self.__ParseData__(44)
        cost_cls = sorted(cost_cls, key = lambda cost: cost[u'region_3point'][0])

        for cost_item in cost_cls:
            cost_confidence_level, cost = CostFixed().StartFixedFromJson(cost_item)
            cost_item[u'fixed'] = cost.strip()
            cost_item[u'fixed_confidence'] = cost_confidence_level.value

        cost_str = ''
        for temp in cost_cls:
            cost_str += re.sub(r' ', u'', temp[u'fixed'])

        cost_str = re.sub(r'[^A-Z0-9\.]', u'', cost_str)
        regex = re.search(r'^[A-Z]', cost_str)
        if not regex:
            cost_str = u'CNY' + cost_str

        regex = re.search(self.__Patterns__, cost_str)
        if regex:
            keys = []
            values = []
            for index in range(5):
                if len(regex.group(index * 2 + 1)):
                    keys.append(regex.group(index * 2 + 1))
                    values.append(regex.group((index + 1) * 2))
            
            fixedcost = {}
            for index in range(len(keys)):
                if keys[index] == u'Y':
                    keys[index] = u'YQ'

                regex = re.search(self.__CostKeysPatterns__, keys[index])
                if regex:
                    keys[index] = regex.group(1)

                try:
                    if index == len(keys) - 1 and float(self.__FixMoney__(values[index])) > float(fixedcost.get(u'basic_fare', '0')):
                        keys[index] = u'total'
                    elif index == len(keys) - 1:
                        # 可能是总价丢了
                        keys[index] = u'YQ'
                except Exception:
                    keys[index] = u'YQ'
                    logging.info(u'Fix Total Cost Error')

                if keys[index] in [u'CNY'] and index != len(keys) - 1:
                    if index == 0:
                        keys[index] = u'fare'
                    elif index == 1:
                        keys[index] = u'CN'
                    elif index == 2:
                        keys[index] = u'XT'

                for mapped_key in self.__KeyMapping__.keys():
                    if keys[index] in self.__KeyMapping__[mapped_key]:
                        keys[index] = mapped_key

                if len(keys[index]) and keys[index] in self.__KeyMapping__.keys():
                    fixedcost[keys[index]] = self.__FixMoney__(values[index])

            # G3, G4, F6, K3, ZF mapping to fuel_surcharge
            regex = re.search(r'(G3|G4|F6|K3|ZF)(\d*)', cost_str)
            if regex and not fixedcost.get(u'fuel_surcharge'):
                fixedcost[u'fuel_surcharge'] = self.__FixMoney__(regex.group(2))

        else:
            logging.info('seperate cost error')

        if len(cost_cls):
            regex = re.search(r'[0-9\.]*', cost_cls[len(cost_cls) - 1][u'fixed'])
            if regex:
                try:
                    total = self.__FixMoney__(cost_cls[len(cost_cls) - 1][u'fixed'])
                    if float(total) > (fixedcost.get(u'basic_fare', '0')):
                        fixedcost[u'after_tax_amount'] = total
                except Exception:
                    logging.info(u'Fix Money Error')

        if not fixedcost.get(u'after_tax_amount'):
            try:
                total = 0
                for value in fixedcost.values():
                    total += float(value)

                fixedcost[u'after_tax_amount'] = u'{:.2f}'.format(total)
            except Exception:
                pass

        return cost_cls, fixedcost

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
