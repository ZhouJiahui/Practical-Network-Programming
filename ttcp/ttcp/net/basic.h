#ifndef TTCP_NET_BASIC_H_
#define TTCP_NET_BASIC_H_


class noncopyable
{
 protected:
  noncopyable() {}

 private:
  noncopyable(const noncopyable&) = delete;
  void operator=(const noncopyable&) = delete;
};

struct copyable
{
};

template<typename To,typename From>
inline To implicit_cast(const From &f)
{
    return f;
}

#endif
