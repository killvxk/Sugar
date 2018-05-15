import os
import json

class DataFixed(object):
    """description of class"""
    def __init__(self, validatedPath, resultPath):
        self.ValidatedPath = validatedPath
        self.ResultPath = resultPath


    def StartFixed(self):
        self.__BeforeFixed__();

        self.__Handler__(self.ValidatedPath, '10101_283.jpg.json')
        self.__VisitFolder__(self.ValidatedPath, self.__Handler__)

        self.__AfterFixed__();


    def __BeforeFixed__(self):
        pass


    def __FixedData__(self, validateJson, resultJson):
        pass


    def __AfterFixed__(self):
        pass


    def __VisitFolder__(self, folderpath, visitor):
        if not os.path.exists(folderpath):
            return

        names = os.listdir(folderpath)
        for name in names:
            if os.path.isdir(folderpath + name):
                subdir = folderpath + name + '/'
                self.__VisitFolder__(subdir)
            else:
                self.__Handler__(folderpath, name)


    def __Handler__(self, filepath, filename):
        print('Start Handler ' + filename)

        validateJson = json.load(open(self.ValidatedPath + filename, encoding='utf-8'))
        resultJson = json.load(open(self.ResultPath + filename, encoding='utf-8'))

        self.__FixedData__(validateJson, resultJson);

        print('End Handler ' + filename + '\n')

