#include <gtest/gtest.h>
#include <cstring>
#include <string>
#include <stdint.h>
#include "ip.h"
#include "tcp.h"
#include "udp.h"
#include "icmp.h"
#include "rawpdu.h"
#include "ip_address.h"
#include "utils.h"
#include "ethernetII.h"

using namespace std;
using namespace Tins;

#ifdef _WIN32
    #define TINS_DEFAULT_TEST_IP "0.0.0.0"
#else 
    #define TINS_DEFAULT_TEST_IP "127.0.0.1"
#endif

class IPTest : public testing::Test {
public:
    static const uint8_t expected_packet[], fragmented_packet[], 
        fragmented_ether_ip_packet[], tot_len_zero_packet[];
    
    void test_equals(const IP &ip1, const IP &ip2);
    void test_overwrite_source_address(IP& ip);
};

const uint8_t IPTest::expected_packet[] = { 
    40, 127, 0, 32, 0, 122, 0, 67, 21, 1, 0, 0, 84, 52, 254, 5, 192, 
    168, 9, 43, 130, 11, 116, 106, 103, 171, 119, 171, 104, 101, 108, 0
};

const uint8_t IPTest::fragmented_packet[] = { 
    69, 0, 0, 60, 0, 242, 7, 223, 64, 17, 237, 220, 192, 0, 2, 1, 192, 
    0, 2, 2, 192, 0, 192, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t IPTest::fragmented_ether_ip_packet[] = {
    0, 10, 94, 83, 216, 229, 0, 21, 197, 50, 245, 6, 8, 0, 69, 0, 0, 60, 
    0, 242, 7, 223, 64, 17, 237, 220, 192, 0, 2, 1, 192, 0, 2, 2, 192, 0, 
    192, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const uint8_t IPTest::tot_len_zero_packet[] = {
    60, 151, 14, 219, 60, 164, 60, 151, 14, 218, 161, 43, 8, 0, 69, 0
    , 0, 0, 29, 214, 64, 0, 128, 6, 0, 0, 192, 168, 1, 20, 192, 168, 
    1, 21, 192, 190, 23, 172, 226, 151, 206, 49, 83, 30, 140, 232, 80
    , 24, 1, 0, 131, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 165, 111, 64, 0, 1, 0, 0, 0, 2, 66, 65, 0, 2
    , 66, 65, 0, 0, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 
    0, 32, 0, 32, 0, 40, 0, 40, 0, 40, 0, 40, 0, 40, 0, 32, 0, 32, 0,
    32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 
    32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 32, 0, 72, 0, 16, 0, 16
    , 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 
    0, 16, 0, 16, 0, 16, 0, 16, 0, 132, 0, 132, 0, 132, 0, 132, 0, 
    132, 0, 132, 0, 132, 0, 132, 0, 132, 0, 132, 0, 16, 0, 16, 0, 16,
    0, 16, 0, 16, 0, 16, 0, 16, 0, 129, 0, 129, 0, 129, 0, 129, 0, 
    129, 0, 129, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1
    , 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 
    1, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 16, 0, 130, 0, 130, 0, 
    130, 0, 130, 0, 130, 0, 130, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 
    0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2,
    0, 2, 0, 2, 0, 2, 0, 16, 0, 16, 0, 16, 0, 16, 0, 32, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 46
    , 0, 0, 0, 1, 0, 0, 0, 120, 16, 65, 0, 104, 16, 65, 0, 255, 255, 
    255, 255, 0, 10, 0, 0, 16, 0, 0, 0, 248, 3, 0, 0, 32, 5, 147, 25,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 192, 11, 0, 0, 0, 0
    , 0, 0, 0, 29, 0, 0, 192, 4, 0, 0, 0, 0, 0, 0, 0, 150, 0, 0, 192,
    4, 0, 0, 0, 0, 0, 0, 0, 141, 0, 0, 192, 8, 0, 0, 0, 0, 0, 0, 0, 
    142, 0, 0, 192, 8, 0, 0, 0, 0, 0, 0, 0, 143, 0, 0, 192, 8, 0, 0, 
    0, 0, 0, 0, 0, 144, 0, 0, 192, 8, 0, 0, 0, 0, 0, 0, 0, 145, 0, 0,
    192, 8, 0, 0, 0, 0, 0, 0, 0, 146, 0, 0, 192, 8, 0, 0, 0, 0, 0, 0
    , 0, 147, 0, 0, 192, 8, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 7, 0, 0,
    0, 10, 0, 0, 0, 140, 0, 0, 0, 2, 0, 0, 0, 240, 18, 65, 0, 8, 0, 
    0, 0, 196, 18, 65, 0, 9, 0, 0, 0, 152, 18, 65, 0, 10, 0, 0, 0, 
    116, 18, 65, 0, 16, 0, 0, 0, 72, 18, 65, 0, 17, 0, 0, 0, 24, 18, 
    65, 0, 18, 0, 0, 0, 244, 17, 65, 0, 19, 0, 0, 0, 200, 17, 65, 0, 
    24, 0, 0, 0, 144, 17, 65, 0, 25, 0, 0, 0, 104, 17, 65, 0, 26, 0, 
    0, 0, 48, 17, 65, 0, 27, 0, 0, 0, 248, 16, 65, 0, 28, 0, 0, 0, 
    208, 16, 65, 0, 120, 0, 0, 0, 192, 16, 65, 0, 121, 0, 0, 0, 176, 
    16, 65, 0, 122, 0, 0, 0, 160, 16, 65, 0, 252, 0, 0, 0, 156, 16, 
    65, 0, 255, 0, 0, 0, 140, 16, 65, 0, 15, 197, 64, 0, 15, 197, 64,
    0, 15, 197, 64, 0, 15, 197, 64, 0, 15, 197, 64, 0, 15, 197, 64, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 4, 8, 0, 0, 0, 0, 164, 3, 0, 0, 
    96, 130, 121, 130, 33, 0, 0, 0, 0, 0, 0, 0, 166, 223, 0, 0, 0, 0,
    0, 0, 161, 165, 0, 0, 0, 0, 0, 0, 129, 159, 224, 252, 0, 0, 0, 0
    , 64, 126, 128, 252, 0, 0, 0, 0, 168, 3, 0, 0, 193, 163, 218, 163
    , 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 129, 254, 0, 0, 0, 0, 0, 0, 64, 254, 0, 0, 0, 0, 0, 0, 
    181, 3, 0, 0, 193, 163, 218, 163, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 129, 254, 0, 0, 0, 0, 0
    , 0, 65, 254, 0, 0, 0, 0, 0, 0, 182, 3, 0, 0, 207, 162, 228, 162,
    26, 0, 229, 162, 232, 162, 91, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 129, 254, 0, 0, 0, 0, 0, 0, 64, 126, 161, 254, 
    0, 0, 0, 0, 81, 5, 0, 0, 81, 218, 94, 218, 32, 0, 95, 218, 106, 
    218, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 129, 
    211, 216, 222, 224, 249, 0, 0, 49, 126, 129, 254, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 22, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0
    , 0, 3, 0, 0, 0, 2, 0, 0, 0, 4, 0, 0, 0, 24, 0, 0, 0, 5, 0, 0, 0,
    13, 0, 0, 0, 6, 0, 0, 0, 9, 0, 0, 0, 7, 0, 0, 0, 12, 0, 0, 0, 8,
    0, 0, 0, 12, 0, 0, 0, 9, 0, 0, 0, 12, 0, 0, 0, 10, 0, 0, 0, 7, 0
    , 0, 0, 11, 0, 0, 0, 8, 0, 0, 0, 12, 0, 0, 0, 22, 0, 0, 0, 13, 0,
    0, 0, 22, 0, 0, 0, 15, 0, 0, 0, 2, 0, 0, 0, 16, 0, 0, 0, 13, 0, 
    0, 0, 17, 0, 0, 0, 18, 0, 0, 0, 18, 0, 0, 0, 2, 0, 0, 0, 33, 0, 0
    , 0, 13, 0, 0, 0, 53, 0, 0, 0, 2, 0, 0, 0, 65, 0, 0, 0, 13, 0, 0,
    0, 67, 0, 0, 0, 2, 0, 0, 0, 80, 0, 0, 0, 17, 0, 0, 0, 82, 0, 0, 
    0, 13, 0, 0, 0, 83, 0, 0, 0, 13, 0, 0, 0, 87, 0, 0, 0, 22, 0, 0, 
    0, 89, 0, 0, 0, 11, 0, 0, 0, 108, 0, 0, 0, 13, 0, 0, 0, 109, 0, 0
    , 0, 32, 0, 0, 0, 112, 0, 0, 0, 28, 0, 0, 0, 114, 0, 0, 0, 9, 0, 
    0, 0, 6, 0, 0, 0, 22, 0, 0, 0, 128, 0, 0, 0, 10, 0, 0, 0, 129, 0,
    0, 0, 10, 0, 0, 0, 130, 0, 0, 0, 9, 0, 0, 0, 131, 0, 0, 0, 22, 0
    , 0, 0, 132, 0, 0, 0, 13, 0, 0, 0, 145, 0, 0, 0, 41, 0, 0, 0, 158
    , 0, 0, 0, 13, 0, 0, 0, 161, 0, 0, 0, 2, 0, 0, 0, 164, 0, 0, 0, 
    11, 0, 0, 0, 167, 0, 0, 0, 13, 0, 0, 0, 183, 0, 0, 0, 17, 0, 0, 0
    , 206, 0, 0, 0, 2, 0, 0, 0, 215, 0, 0, 0, 11, 0, 0, 0, 24, 7, 0, 
    0, 12, 0, 0, 0, 128, 112, 0, 0, 1, 0, 0, 0, 240, 241, 255, 255, 
    80, 83, 84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    80, 68, 84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    228, 71, 65, 0, 36, 72, 65, 0, 0, 0, 0, 0, 255, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0,
    0, 0, 0, 0, 255, 255, 255, 255, 30, 0, 0, 0, 59, 0, 0, 0, 90, 0,
    0, 0, 120, 0, 0, 0, 151, 0, 0, 0, 181, 0, 0, 0, 212, 0, 0, 0, 
    243, 0, 0, 0, 17, 1, 0, 0, 48, 1, 0, 0, 78, 1, 0, 0, 109, 1, 0, 0
    , 255, 255, 255, 255, 30, 0, 0, 0, 58, 0, 0, 0, 89, 0, 0, 0, 119,
    0, 0, 0, 150, 0, 0, 0, 180, 0, 0, 0, 211, 0, 0, 0, 242, 0, 0, 0,
    16, 1, 0, 0, 47, 1, 0, 0, 77, 1, 0, 0, 108, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 13, 0, 13, 0, 10, 0, 0, 166, 53, 
    0, 47, 0, 63, 0, 0, 149, 0, 164, 71, 0, 224, 71, 224, 71, 224, 
    119, 0, 151, 72, 0, 224, 72, 224, 72, 224, 141, 0, 152, 73, 0, 
    224, 73, 224, 73, 224, 134, 0, 153, 75, 0, 224, 75, 224, 75, 224,
    115, 0, 155, 77, 0, 224, 77, 224, 77, 224, 116, 0, 157, 79, 0, 
    224, 79, 224, 79, 224, 117, 0, 159, 80, 0, 224, 80, 224, 80, 224,
    145, 0, 160, 81, 0, 224, 81, 224, 81, 224, 118, 0, 161, 82, 0, 
    224, 82, 224, 82, 224, 146, 0, 162, 83, 0, 224, 83, 224, 83, 224,
    147, 0, 163, 0, 0, 0, 0, 0, 0, 0, 0, 27, 0, 27, 0, 27, 0, 0, 1, 
    49, 0, 33, 0, 0, 0, 0, 120, 50, 0, 64, 0, 0, 3, 0, 121, 51, 0, 35
    , 0, 0, 0, 0, 122, 52, 0, 36, 0, 0, 0, 0, 123, 53, 0, 37, 0, 0, 0
    , 0, 124, 54, 0, 94, 0, 30, 0, 0, 125, 55, 0, 38, 0, 0, 0, 0, 126
    , 56, 0, 42, 0, 0, 0, 0, 127, 57, 0, 40, 0, 0, 0, 0, 128, 48, 0, 
    41, 0, 0, 0, 0, 129, 45, 0, 95, 0, 31, 0, 0, 130, 61, 0, 43, 0, 0
    , 0, 0, 131, 8, 0, 8, 0, 127, 0, 0, 14, 9, 0, 0, 15, 0, 148, 0, 
    15, 113, 0, 81, 0, 17, 0, 0, 16, 119, 0, 87, 0, 23, 0, 0, 17, 101
    , 0, 69, 0, 5, 0, 0, 18, 114, 0, 82, 0, 18, 0, 0, 19, 116, 0, 84,
    0, 20, 0, 0, 20, 121, 0, 89, 0, 25, 0, 0, 21, 117, 0, 85, 0, 21,
    0, 0, 22, 105, 0, 73, 0, 9, 0, 0, 23, 111, 0, 79, 0, 15, 0, 0, 
    24, 112, 0, 80, 0, 16, 0, 0, 25, 91, 0, 123, 0, 27, 0, 0, 26, 93,
    0, 125, 0, 29, 0, 0, 27, 13, 0, 13, 0, 10, 0, 0, 28, 0, 0, 0, 0,
    0, 0, 0, 0, 97, 0, 65, 0, 1, 0, 0, 30, 115, 0, 83, 0, 19, 0, 0, 
    31, 100, 0, 68, 0, 4, 0, 0, 32, 102, 0, 70, 0, 6, 0, 0, 33, 103, 
    0, 71, 0, 7, 0, 0, 34, 104, 0, 72, 0, 8, 0, 0, 35, 106, 0, 74, 0,
    10, 0, 0, 36, 107, 0, 75, 0, 11, 0, 0, 37, 108, 0, 76, 0, 12, 0,
    0, 38, 59, 0, 58, 0, 0, 0, 0, 39, 39, 0, 34, 0, 0, 0, 0, 40, 96,
    0, 126, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 92, 0, 124, 0, 
    28, 0, 0, 0, 122, 0, 90, 0, 26, 0, 0, 44, 120, 0, 88, 0, 24, 0, 0
    , 45, 99, 0, 67, 0, 3, 0, 0, 46, 118, 0, 86, 0, 22, 0, 0, 47, 98,
    0, 66, 0, 2, 0, 0, 48, 110, 0, 78, 0, 14, 0, 0, 49, 109, 0, 77, 
    0, 13, 0, 0, 50, 44, 0, 60, 0, 0, 0, 0, 51, 46, 0, 62, 0, 0, 0, 0
    , 52, 47, 0, 63, 0, 0, 0, 0, 53, 0, 0, 0, 0, 0, 0, 0, 0, 42, 0, 0
    , 0, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 32, 0, 32, 0, 
    32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59, 0, 84, 0, 94, 0, 104, 0, 60
    , 0, 85, 0, 95, 0, 105, 0, 61, 0, 86, 0, 96, 0, 106, 0, 62, 0, 87
    , 0, 97, 0, 107, 0, 63, 0, 88, 0, 98, 0, 108, 0, 64, 0, 89, 0, 99
    , 0, 109, 0, 65, 0, 90, 0, 100, 0, 110, 0, 66, 0, 91, 0, 101, 0, 
    111, 0, 67, 0, 92, 0, 102, 0, 112, 0, 68, 0, 93, 0, 103, 0, 113, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 71, 55, 0, 0, 
    119, 0, 0, 0, 72, 56, 0, 0, 141, 0, 0, 0, 73, 57, 0, 0, 132, 0, 0
    , 0, 0, 45, 0, 0, 0, 0, 0, 0, 75, 52, 0, 0, 115, 0, 0, 0, 0, 53, 
    0, 0, 0, 0, 0, 0, 77, 54, 0, 0, 116, 0, 0, 0, 0, 43, 0, 0, 0, 0, 
    0, 0, 79, 49, 0, 0, 117, 0, 0, 0, 80, 50, 0, 0, 145, 0, 0, 0, 81,
    51, 0, 0, 118, 0, 0, 0, 82, 48, 0, 0, 146, 0, 0, 0, 83, 46, 0, 0
    , 147, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 224, 133, 224, 135, 224, 137, 224, 139, 224, 
    134, 224, 136, 224, 138, 224, 140, 255, 255, 255, 255, 254, 255, 
    255, 255, 254, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    , 0, 0, 0, 0, 0
};


TEST_F(IPTest, DefaultConstructor) {
    IP ip;
    EXPECT_EQ(ip.dst_addr(), "0.0.0.0");
    EXPECT_EQ(ip.src_addr(), "0.0.0.0");
    EXPECT_EQ(ip.version(), 4);
    EXPECT_EQ(ip.id(), 1);
    EXPECT_EQ(ip.pdu_type(), PDU::IP);
}

TEST_F(IPTest, CopyConstructor) {
    IP ip1(expected_packet, sizeof(expected_packet));
    IP ip2(ip1);
    test_equals(ip1, ip2);
}

TEST_F(IPTest, CopyAssignmentOperator) {
    IP ip1(expected_packet, sizeof(expected_packet));
    IP ip2;
    ip2 = ip1;
    test_equals(ip1, ip2);
}

TEST_F(IPTest, NestedCopy) {
    IP *nested = new IP(expected_packet, sizeof(expected_packet));
    IP ip1;
    ip1.inner_pdu(nested);
    IP ip2(ip1);
    test_equals(ip1, ip2);
}

TEST_F(IPTest, Constructor) {
    IP ip("192.168.0.1", "192.168.0.100");
    EXPECT_EQ(ip.dst_addr(), "192.168.0.1");
    EXPECT_EQ(ip.src_addr(), "192.168.0.100");
    EXPECT_EQ(ip.version(), 4);
    EXPECT_EQ(ip.id(), 1);
}

TEST_F(IPTest, ConstructorFromFragmentedPacket) {
    IP ip(fragmented_packet, sizeof(fragmented_packet));
    ASSERT_TRUE(ip.inner_pdu() != NULL);
    EXPECT_EQ(PDU::RAW, ip.inner_pdu()->pdu_type());
}

TEST_F(IPTest, SerializeFragmentedPacket) {
    EthernetII pkt(fragmented_ether_ip_packet, sizeof(fragmented_ether_ip_packet));
    PDU::serialization_type buffer = pkt.serialize();
    EXPECT_EQ(
        PDU::serialization_type(
            fragmented_ether_ip_packet, 
            fragmented_ether_ip_packet + sizeof(fragmented_ether_ip_packet)
        ),
        buffer
    );
}

TEST_F(IPTest, TotalLengthZeroPacket) {
    EthernetII pkt(tot_len_zero_packet, sizeof(tot_len_zero_packet));
    ASSERT_TRUE(pkt.find_pdu<TCP>() != NULL);
    ASSERT_TRUE(pkt.find_pdu<RawPDU>() != NULL);
    EXPECT_EQ(8192U, pkt.rfind_pdu<RawPDU>().payload_size());
}

TEST_F(IPTest, TOS) {
    IP ip;
    ip.tos(0x7a);
    EXPECT_EQ(ip.tos(), 0x7a);
}

TEST_F(IPTest, ID) {
    IP ip;
    ip.id(0x7f1a);
    EXPECT_EQ(ip.id(), 0x7f1a);
}

TEST_F(IPTest, FragOffset) {
    IP ip;
    ip.frag_off(0x7f1a);
    EXPECT_EQ(ip.frag_off(), 0x7f1a);
}

TEST_F(IPTest, TTL) {
    IP ip;
    ip.ttl(0x7f);
    EXPECT_EQ(ip.ttl(), 0x7f);
}

TEST_F(IPTest, Protocol) {
    IP ip;
    ip.protocol(0x7f);
    EXPECT_EQ(ip.protocol(), 0x7f);
}

TEST_F(IPTest, SrcIPString) {
    IP ip;
    string string_ip("192.155.32.10");
    ip.src_addr(string_ip);
    EXPECT_EQ(ip.src_addr(), IPv4Address(string_ip));
}

TEST_F(IPTest, DstIPString) {
    IP ip;
    string string_ip("192.155.32.10");
    ip.dst_addr(string_ip);
    EXPECT_EQ(ip.dst_addr(), IPv4Address(string_ip));
}

TEST_F(IPTest, SrcIPInt) {
    IP ip;
    ip.src_addr("192.155.32.10");
    EXPECT_EQ(ip.src_addr(), "192.155.32.10");
}

TEST_F(IPTest, DstIPInt) {
    IP ip;
    ip.dst_addr("192.155.32.10");
    EXPECT_EQ(ip.dst_addr(), "192.155.32.10");
}

TEST_F(IPTest, Version) {
    IP ip;
    ip.version(0xb);
    EXPECT_EQ(ip.version(), 0xb);
}

TEST_F(IPTest, SecOption) {
    IP ip;
    ip.security(IP::security_type(0x746a, 26539, 0x77ab, 0x68656c));
    IP::security_type found = ip.security();
    EXPECT_EQ(found.security, 0x746a);
    EXPECT_EQ(found.compartments, 26539);
    EXPECT_EQ(found.handling_restrictions, 0x77ab);
    EXPECT_EQ(found.transmission_control, 0x68656cU);
}

TEST_F(IPTest, LSRROption) {
    IP ip;
    IP::lsrr_type lsrr(0x2d);
    lsrr.routes.push_back("192.168.2.3");
    lsrr.routes.push_back("192.168.5.1");
    ip.lsrr(lsrr);
    IP::lsrr_type found = ip.lsrr();
    EXPECT_EQ(found.pointer, lsrr.pointer);
    EXPECT_EQ(found.routes, lsrr.routes);
}

TEST_F(IPTest, SSRROption) {
    IP ip;
    IP::ssrr_type ssrr(0x2d);
    ssrr.routes.push_back("192.168.2.3");
    ssrr.routes.push_back("192.168.5.1");
    ip.ssrr(ssrr);
    IP::ssrr_type found = ip.ssrr();
    EXPECT_EQ(found.pointer, ssrr.pointer);
    EXPECT_EQ(found.routes, ssrr.routes);
}

TEST_F(IPTest, RecordRouteOption) {
    IP ip;
    IP::record_route_type record_route(0x2d);
    record_route.routes.push_back("192.168.2.3");
    record_route.routes.push_back("192.168.5.1");
    ip.record_route(record_route);
    IP::record_route_type found = ip.record_route();
    EXPECT_EQ(found.pointer, record_route.pointer);
    EXPECT_EQ(found.routes, record_route.routes);
}

TEST_F(IPTest, StreamIDOption) {
    IP ip;
    ip.stream_identifier(0x91fa);
    EXPECT_EQ(0x91fa, ip.stream_identifier());
}

TEST_F(IPTest, AddOption) {
    IP ip;
    const uint8_t data[] = { 0x15, 0x17, 0x94, 0x66, 0xff };
    IP::option_identifier id(IP::SEC, IP::CONTROL, 1);
    ip.add_option(IP::option(id, data, data + sizeof(data)));
    const IP::option *opt = ip.search_option(id);
    ASSERT_TRUE(opt != NULL);
    ASSERT_EQ(opt->data_size(), sizeof(data));
    EXPECT_TRUE(memcmp(opt->data_ptr(), data, sizeof(data)) == 0);
}

void IPTest::test_equals(const IP &ip1, const IP &ip2) {
    EXPECT_EQ(ip1.dst_addr(), ip2.dst_addr());
    EXPECT_EQ(ip1.src_addr(), ip2.src_addr());
    EXPECT_EQ(ip1.id(), ip2.id());
    EXPECT_EQ(ip1.frag_off(), ip2.frag_off());
    EXPECT_EQ(ip1.tos(), ip2.tos());
    EXPECT_EQ(ip1.ttl(), ip2.ttl());
    EXPECT_EQ(ip1.version(), ip2.version());
    EXPECT_EQ(ip1.inner_pdu() != NULL, ip2.inner_pdu() != NULL);
}

TEST_F(IPTest, ConstructorFromBuffer) {
    IP ip(expected_packet, sizeof(expected_packet));
    
    EXPECT_EQ(ip.dst_addr(), "192.168.9.43");
    EXPECT_EQ(ip.src_addr(), "84.52.254.5");
    EXPECT_EQ(ip.id(), 0x7a);
    EXPECT_EQ(ip.tos(), 0x7f);
    EXPECT_EQ(ip.frag_off(), 0x43);
    EXPECT_EQ(ip.protocol(), 1);
    EXPECT_EQ(ip.ttl(), 0x15);
    EXPECT_EQ(ip.version(), 2);
    
    IP::security_type sec = ip.security();
    EXPECT_EQ(sec.security, 0x746a);
    EXPECT_EQ(sec.compartments, 26539);
    EXPECT_EQ(sec.handling_restrictions, 0x77ab);
    EXPECT_EQ(sec.transmission_control, 0x68656cU);
}

TEST_F(IPTest, StackedProtocols) {
    IP ip = IP(TINS_DEFAULT_TEST_IP) / TCP();
    IP::serialization_type buffer = ip.serialize();
    EXPECT_TRUE(IP(&buffer[0], (uint32_t)buffer.size()).find_pdu<TCP>() != NULL);
    
    ip = IP(TINS_DEFAULT_TEST_IP) / UDP();
    buffer = ip.serialize();
    EXPECT_TRUE(IP(&buffer[0], (uint32_t)buffer.size()).find_pdu<UDP>() != NULL);
    
    ip = IP(TINS_DEFAULT_TEST_IP) / ICMP();
    buffer = ip.serialize();
    EXPECT_TRUE(IP(&buffer[0], (uint32_t)buffer.size()).find_pdu<ICMP>() != NULL);
}

TEST_F(IPTest, SpoofedOptions) {
    IP pdu(TINS_DEFAULT_TEST_IP);
    uint8_t a[] = { 1,2,3,4,5,6 };
    pdu.add_option(
        IP::option(IP::NOOP, 250, a, a + sizeof(a))
    );
    pdu.add_option(
        IP::option(IP::NOOP, 250, a, a + sizeof(a))
    );
    pdu.add_option(
        IP::option(IP::NOOP, 250, a, a + sizeof(a))
    );
    // probably we'd expect it to crash if it's not working, valgrind plx
    EXPECT_EQ(3U, pdu.options().size());
    EXPECT_EQ(pdu.serialize().size(), pdu.size());
}

TEST_F(IPTest, RemoveOption) {
    IP ip(TINS_DEFAULT_TEST_IP);
    PDU::serialization_type old_buffer = ip.serialize();

    // Add a record route option
    IP::record_route_type record_route(0x2d);
    record_route.routes.push_back("192.168.2.3");
    record_route.routes.push_back("192.168.5.1");
    ip.record_route(record_route);

    // Add a lsrr option
    IP::lsrr_type lsrr(0x2d);
    lsrr.routes.push_back("192.168.2.3");
    lsrr.routes.push_back("192.168.5.1");
    ip.lsrr(lsrr);
    
    EXPECT_TRUE(ip.remove_option(IP::option_identifier(IP::LSRR, IP::CONTROL, 1)));
    EXPECT_TRUE(ip.remove_option(IP::option_identifier(IP::RR, IP::CONTROL, 0)));

    PDU::serialization_type new_buffer = ip.serialize();
    EXPECT_EQ(old_buffer, new_buffer);
}

void IPTest::test_overwrite_source_address(IP& ip) {
    const uint8_t expected_output[] = {
        69,0,0,40,0,1,0,0,128,6,38,186,1,2,3,4,8,8,8,8,0,32,0,12,0,
        0,0,0,0,0,0,0,80,0,127,166,27,253,0,0
    };
    // Add TCP so we can check if the timestamp is correctly calculated
    ip /= TCP(12, 32);

    PDU::serialization_type buffer = ip.serialize();
    EXPECT_EQ(IPv4Address("1.2.3.4"), ip.src_addr());
    EXPECT_EQ(0x26ba, ip.checksum());

    vector<uint8_t> output_buffer(expected_output, 
                                  expected_output + sizeof(expected_output));
    EXPECT_EQ(output_buffer, buffer);
}

TEST_F(IPTest, OverwriteSourceAddress) {
    IP ip("8.8.8.8");
    ip.src_addr("1.2.3.4");
    test_overwrite_source_address(ip);
}

TEST_F(IPTest, OverwriteSourceAddressUsingConstructor) {
    IP ip("8.8.8.8", "1.2.3.4");
    test_overwrite_source_address(ip);
}

TEST_F(IPTest, OverwriteSourceAddressConstructingFromBuffer) {
    // Only the expected packet's IP layer
    const uint8_t expected_output[] = {
        69,0,0,20,0,1,0,0,128,0,0,0,1,2,3,4,8,8,8,8
    };
    IP ip(expected_output, sizeof(expected_output));
    test_overwrite_source_address(ip);
}


