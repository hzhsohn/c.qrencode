message(-> libqrencode库)

#所有的路径变量都不允许重名
AUX_SOURCE_DIRECTORY(. libqrencode_SRC_LIST)
#设置成静态库
ADD_LIBRARY(libqrencode ${libqrencode_SRC_LIST})