#ifndef MEMORY_AS_H_
# define MEMORY_AS_H_

# include <algorithm>
# include <stdexcept>
# include <map>
# include <utility>

# include <stdint.h>

namespace gbemu { namespace memory {

class as
{
  private:
    /*
    ** This template parameter is not really useful in fact, as it always will
    ** be uint8_t.
    */
    template<typename T>
    class rw_proxy
    {
      public:
        rw_proxy(std::function<T (uint16_t)> read_handler,
                 std::function<void (uint16_t, T)> write_handler,
                 uint16_t read_offset, uint16_t write_offset)
          : read_handler_(read_handler), write_handler_(write_handler),
            read_offset_(read_offset), write_offset_(write_offset)
        {
        }

        operator T()
        {
          return this->read_handler_(this->read_offset_);
        }

        rw_proxy& operator=(const T& val)
        {
          this->write_handler_(this->write_offset_, val);
          return *this;
        }

      private:
        std::function<T (uint16_t)> read_handler_;
        std::function<void (uint16_t, T)> write_handler_;
        uint16_t read_offset_;
        uint16_t write_offset_;
    };

  public:
    void add_read_handler(std::pair<uint16_t, uint16_t> range,
                          std::function<uint8_t (uint16_t)> handler)
    {
      this->read_handlers_[range] = handler;
    }

    void add_write_handler(std::pair<uint16_t, uint16_t> range,
                           std::function<void (uint16_t, uint8_t)> handler)
    {
      this->write_handlers_[range] = handler;
    }

    rw_proxy<uint8_t> operator[](uint16_t addr) const
    {
      std::function<uint8_t (uint16_t)> rh;
      std::function<void (uint16_t, uint8_t)> wh;
      uint16_t read_offset;
      uint16_t write_offset;

      /*
      ** These handlers are meant to throw an exception if the proxy is ever
      ** used on an invalid range.
      ** We do not try to detect not found handlers here because a range can
      ** be, for example, write-only, and as long as the user does not try to
      ** read in it, everything is fine.
      */
      rh = [](uint16_t) -> uint8_t { throw std::out_of_range("invalid memory read"); };
      wh = [](uint16_t, uint8_t){ throw std::out_of_range("invalid memory write"); };

      for (auto& e : read_handlers_)
      {
        auto range = e.first;
        auto handler = e.second;

        if (range.first <= addr && addr <= range.second)
        {
          rh = handler;
          read_offset = addr - range.first;
          break;
        }
      }

      for (auto& e : write_handlers_)
      {
        auto range = e.first;
        auto handler = e.second;

        if (range.first <= addr && addr <= range.second)
        {
          wh = handler;
          write_offset = addr - range.first;
          break;
        }
      }

      return rw_proxy<uint8_t>(rh, wh, read_offset, write_offset);
    }

  private:
    std::map<
      std::pair<uint16_t, uint16_t>,
      std::function<uint8_t (uint16_t)>
    > read_handlers_;

    std::map<
      std::pair<uint16_t, uint16_t>,
      std::function<void (uint16_t, uint8_t)>
    > write_handlers_;
};

}}

#endif /* !MEMORY_AS_H_ */
