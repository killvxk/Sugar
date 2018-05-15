def Similarity(str1, str2):
    len1 = len(str1)
    len2 = len(str2)
    diff = 0
    diffarray = [([0] * (len2 + 1)) for index in range((len1+ 1))]

    for index in range(len1 + 1):
        diffarray[index][0] = index

    for index in range(len2 + 1):
        diffarray[0][index] = index

    diff = 0

    for i in range(1, len1 + 1):
        for j in range(1, len2 + 1):
            if str1[i - 1] == str2[j - 1]:
                diff = 0
            else:
                diff = 1
            
            diffarray[i][j] = min(min(diffarray[i - 1][j - 1] + diff, diffarray[i][j - 1] + 1), diffarray[i - 1][j] + 1)

    return diffarray[len1][len2]

def SimilarityRate(str1, str2):
    return 1.0 - Similarity(str1, str2) / max(len(str1), len(str2))
