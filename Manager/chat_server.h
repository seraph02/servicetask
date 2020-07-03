#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "chat_message.h"
#include "manager_listen.h"
#include "stdio.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

class chat_participant
{
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0; //后面须要重载
};

typedef boost::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
  void join(chat_participant_ptr participant)
  {
    participants_.insert(participant);
    std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
        boost::bind(&chat_participant::deliver, participant, _1));
  }

  void leave(chat_participant_ptr participant)
  {
    participants_.erase(participant);

  }


//将从某个client收到的消息挂到 每个client的write_msgs队尾 详细见chat_participant::deliver

  void deliver(const chat_message& msg)

  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front(); //room中保存的消息数有限

    std::for_each(participants_.begin(), participants_.end(),
        boost::bind(&chat_participant::deliver, _1, boost::ref(msg)));
  }

private:
  std::set<chat_participant_ptr> participants_;//用set来保存用户信息
  enum { max_recent_msgs = 1 };
  chat_message_queue recent_msgs_;//用来保存从某个client接收到的信息
};

//----------------------------------------------------------------------

class chat_session
  : public chat_participant,
    public boost::enable_shared_from_this<chat_session>
{
public:
  chat_session(boost::asio::io_service& io_service, chat_room& room)
    : socket_(io_service),
      room_(room)
  {
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()//每生成一个新的chat_session都会调用
  {
    room_.join(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        boost::bind(
          &chat_session::handle_read_header, shared_from_this(),
          boost::asio::placeholders::error)); //异步读client发来的消息
  }

  void deliver(const chat_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg); //把room中保存的消息挂到write_msgs队尾
    if (!write_in_progress)
    {
      boost::asio::async_write(socket_,
          boost::asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
          boost::bind(&chat_session::handle_write, shared_from_this(),
            boost::asio::placeholders::error));
    }
  }

  void handle_read_header(const boost::system::error_code& error)
  {
    if (!error && read_msg_.decode_header())
    {
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
          boost::bind(&chat_session::handle_read_body, shared_from_this(),
            boost::asio::placeholders::error));
    }
    else
    {
      room_.leave(shared_from_this());
    }
  }

  void handle_read_body(const boost::system::error_code& error)
  {
    if (!error)
    {
      try
      {
            string msg = read_msg_.data();
            string ip = socket_.remote_endpoint().address().to_string();
            int len = 0;
            if(msg.length() >5)
            {
                string len_str=msg.substr(0,5);
                len = atoi(len_str.c_str());
                if(len > 0)
                {
                    string t_msg = msg.substr(5,len);
                    Manager_Listen::getInstance()->RecvMsg(ip,t_msg,len);

                }

            }

        //std::cout<<socket_.remote_endpoint().address()<<": "<<read_msg_.data()<<std::endl;
      }
      catch(...){}
      room_.deliver(read_msg_);
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.data(), chat_message::header_length),
          boost::bind(&chat_session::handle_read_header, shared_from_this(),
            boost::asio::placeholders::error));
    }
    else
    {
      room_.leave(shared_from_this());
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      write_msgs_.pop_front();
      if (!write_msgs_.empty())
      {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msgs_.front().data(),
              write_msgs_.front().length()),
            boost::bind(&chat_session::handle_write, shared_from_this(),
              boost::asio::placeholders::error)); //服务器端将收到的消息送给全部的client（广播的方式）
      }
    }
    else
    {
      room_.leave(shared_from_this());
    }
  }

private:
  tcp::socket socket_;
  chat_room& room_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};

typedef boost::shared_ptr<chat_session> chat_session_ptr;

//----------------------------------------------------------------------

class chat_server
{
public:
  chat_server(boost::asio::io_service& io_service,
      const tcp::endpoint& endpoint)
    : io_service_(io_service),
      acceptor_(io_service, endpoint) //全局仅仅有一个io_service和一个acceptor
  {
    start_accept();
  }

  void start_accept()
  {
    chat_session_ptr new_session(new chat_session(io_service_, room_));
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&chat_server::handle_accept, this, new_session,
          boost::asio::placeholders::error));
  }

  void handle_accept(chat_session_ptr session,
      const boost::system::error_code& error)
  {
    if (!error)
    {
      session->start();
    }

    start_accept(); //每连接上一个socket都会调用
  }

private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  chat_room room_; //chat_room中没有重载构造函数 所以会直接调用默认构造函数
};

typedef boost::shared_ptr<chat_server> chat_server_ptr;
typedef std::list<chat_server_ptr> chat_server_list;


#endif // CHAT_SERVER_H
