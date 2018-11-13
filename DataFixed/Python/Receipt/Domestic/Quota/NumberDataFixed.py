import logging

from GeneralNumberDataFixed import GeneralNumberDataFixed
from ConfidenceLevel import ConfidenceLevel

class NumberDataFixed(GeneralNumberDataFixed):
    """description of class"""

    def __init__(self):
        GeneralNumberDataFixed.__init__(self)
        self.__NumberCount__ = 8
        self.__Cls__ = 2
