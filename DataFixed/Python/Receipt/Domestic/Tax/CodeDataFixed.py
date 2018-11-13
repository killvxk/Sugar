import logging

from GeneralNumberDataFixed import GeneralNumberDataFixed
from ConfidenceLevel import ConfidenceLevel

class CodeDataFixed(GeneralNumberDataFixed):
    """description of class"""

    def __init__(self):
        GeneralNumberDataFixed.__init__(self, 'Code')
        self.__NumberCount__ = 12

