#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>

#define LOG_INFO  std::cout << "INFO  (" << __FILE__ << " , " << __LINE__ << " ): "
#define LOG_WARN  std::cout << "WARN  (" << __FILE__ << " , " << __LINE__ << " ): "
#define LOG_ERROR std::cerr << "ERROR (" << __FILE__ << " , " << __LINE__ << " ): "

#endif // __LOG_H__