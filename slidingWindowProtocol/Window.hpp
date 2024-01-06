#include <vector>

class Window {
  
public:
  Window(int size_, int len);
  ~Window();
  void shift();
  void setData(char *begin, size_t len, int pos);
 
  int size;
  int startIdx;
  char **segmentData;
  bool *isSegmentReceived;
};