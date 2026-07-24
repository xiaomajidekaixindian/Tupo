#include "tupo/net/InetAddress.h"
#include <gtest/gtest.h>

class InetAddressTest : public testing::Test {
protected:
  void SetUp() override {}

  void TearDown() override {}
};

// 测试1：测试端口+回环标志构造函数
TEST_F(InetAddressTest, ConstructorWithPortAndLoopback){
  //测试本地回环
  Tupo::net::InetAddress addr1(8080, true);
  const struct sockaddr_in* addr_in1 = addr1.getSockAddrIn();
  EXPECT_EQ(addr_in1->sin_family, AF_INET);
  EXPECT_EQ(addr_in1->sin_port, htons(8080));
  EXPECT_EQ(addr_in1->sin_addr.s_addr, htonl(INADDR_LOOPBACK));

  //测试任意地址
  Tupo::net::InetAddress addr2(8080, false);
  const struct sockaddr_in* addr_in2 = addr2.getSockAddrIn();
  EXPECT_EQ(addr_in2->sin_family, AF_INET);
  EXPECT_EQ(addr_in2->sin_port, htons(8080));
  EXPECT_EQ(addr_in2->sin_addr.s_addr, htonl(INADDR_ANY));
}

// 测试2：测试IP+端口构造函数
TEST_F(InetAddressTest, ConstructorWithIPAndPort){
  //测试IPv4地址
  Tupo::net::InetAddress addr1("127.0.0.1", 8080);
  const struct sockaddr_in* addr_in1 = addr1.getSockAddrIn();
  EXPECT_EQ(addr_in1->sin_family, AF_INET);
  EXPECT_EQ(addr_in1->sin_port, htons(8080));

  char ip_str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &addr_in1->sin_addr, ip_str, sizeof(ip_str));
  EXPECT_STREQ(ip_str, "127.0.0.1");
}

// 测试3：测试IPv4封装构造函数
TEST_F(InetAddressTest, ConstructorWithSockaddrIn){
  struct sockaddr_in addr_in;
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = htons(8080);
  inet_pton(AF_INET, "127.0.0.1", &addr_in.sin_addr);
  Tupo::net::InetAddress addr1(addr_in);
  const struct sockaddr_in* addr_in1 = addr1.getSockAddrIn();
  EXPECT_EQ(addr_in1->sin_family, AF_INET);
  EXPECT_EQ(addr_in1->sin_port, htons(8080));
  EXPECT_EQ(addr_in1->sin_addr.s_addr, addr_in.sin_addr.s_addr);
}

// 测试4：测试IPv6封装构造函数
TEST_F(InetAddressTest, ConstructorWithSockaddrIn6){
  struct sockaddr_in6 addr_in6;
  addr_in6.sin6_family = AF_INET6;
  addr_in6.sin6_port = htons(8080);
  inet_pton(AF_INET6, "::1", &addr_in6.sin6_addr);
  Tupo::net::InetAddress addr1(addr_in6);
  const struct sockaddr_in6* addr_in1 = reinterpret_cast<const struct sockaddr_in6*>(addr1.getSockAddr());
  EXPECT_EQ(addr_in1->sin6_family, AF_INET6);
  EXPECT_EQ(addr_in1->sin6_port, htons(8080));
  EXPECT_EQ(memcmp(&addr_in1->sin6_addr, &addr_in6.sin6_addr, sizeof(addr_in6.sin6_addr)), 0);
}