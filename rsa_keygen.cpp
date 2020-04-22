#include <random>
#include <cmath>
#include <cassert>
#include <iostream>
#include <tuple>

void run_tests();
typedef unsigned long long ull_t;
typedef signed long long sll_t;

// find d,x,y for d = gcd(a,b) = xa+yb
std::tuple<ull_t, sll_t, sll_t> gcd_extended(ull_t a, ull_t b) {

    assert(a>=0);
    assert(b>=0);

    // assert(a>b);

    if (b==0) {
        return std::make_tuple(a, 1, 0);
    }

    ull_t x1 = 0;
    ull_t x2 = 1;
    ull_t y1 = 1;
    ull_t y2 = 0;

    ull_t q, r, x, y;

    while (b>0) {
        q = a/b;
        r = a - q*b;
        x = x2 - q*x1;
        y = y2 - q*y1;

        a = b;
        b = r;
        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;
    }   

    assert(a==1); 
    
    return std::make_tuple(a,x2,y2);
}


// greatest common divisor
ull_t gcd(ull_t a, ull_t b) {

    if (a<b) {
        // swap a and b
        a = a^b;
        b = a^b;
        a = a^b;
    }

    while (b > 0) {
        ull_t r = a % b;
        a = b;
        b = r;
    }
    return a;
}

// Calculates a^b % c 
ull_t power_mod(ull_t a, ull_t b, ull_t c) {
    
    ull_t res = 1; 
    a = a % c;   
  
    while (b > 0) { 
        if (b & 1) { 
            res = (res*a) % c;
        } 
        b = b>>1; 
        a = (a*a) % c;
    } 
    return res; 
} 

std::pair<ull_t, ull_t> factorize_n(ull_t n) {
    // n = 2^r * d + 1
    assert((ull_t)std::log2(n) > 0);

    ull_t r;
    ull_t d;
    for (r=0; r<std::log2(n); r++) {
        d = (n-1) / powl(2,r);

        if (d&1 == 1) {
            // d odd
            assert(pow(2,r)*d+1 == n);
            return std::pair<ull_t, ull_t>(r,d);
        }
    }
    assert(false);
}

// checks if n is probably prime
// Miller Rabin test
ull_t check_prime(ull_t n, int k=10) {

    // n odd
    assert(n&1==1);

    static std::default_random_engine rng;
    std::uniform_int_distribution<ull_t> dist(2, n-2); 

    std::pair<ull_t, ull_t> fact = factorize_n(n);
    ull_t r = fact.first;
    ull_t d = fact.second;

    for (size_t i=0; i<k; i++) {

        ull_t a  = dist(rng);  
        assert(a>=2);
        assert(a<=n-2);

        ull_t x = power_mod(a, d, n);

        if (x==1 || x==n-1) {
            goto WitnessLoop;
        }

        for (size_t j=0; j<r-1; j++) {
            x = power_mod(x, 2, n);
            if (x == n-1) {
                goto WitnessLoop;
            }
        }
        return false;
    WitnessLoop: ;
    }
    return true;
}

ull_t gen_prime(int bit_length, int num_prime_tests=10) {

    static std::default_random_engine rng;
    std::uniform_int_distribution<ull_t> dist(2<<(bit_length-2), (2<<bit_length-1)-1); 
    ull_t x  = dist(rng) | 1;

    while (!check_prime(x, num_prime_tests)) {
        x += 2;
    }
    return x;
}

std::tuple<ull_t, ull_t> rsa_keygen(int k, int e, int num_prime_tests) {

    ull_t p;
    do {
        p = gen_prime(k/2, num_prime_tests);
    } while (p%e == 1);

    ull_t q;
    do {
        q = gen_prime(k-k/2, num_prime_tests);
    } while(q%e == 1);

    ull_t n = p*q;
    ull_t phi = (p-1)*(q-1);

    assert(gcd(e,p-1) == 1);
    assert(gcd(e,phi) == 1);

    auto [v, d, y] = gcd_extended(e,phi);

    return std::make_tuple(n,d);
}

std::vector<ull_t> rsa_encrypt(std::string message, int k=32, int e=17, int num_prime_tests=10) {

    auto [n, d] = rsa_keygen(k, e, num_prime_tests);

    std::vector<ull_t> message_encrypted;
    for (auto m : message) {
        ull_t c = power_mod(m, e, n);
        message_encrypted.push_back(c);
    }
}

int main() {

    run_tests();

    int num_prime_tests = 10;

    int k = 32; // num bits
    int e = 17; // 65537;

    auto [n, d] = rsa_keygen(k, e, num_prime_tests);

    // encrypt message
    std::string message = "The World Wonders";
    std::vector<ull_t> message_encrypted;
    for (auto m : message) {
        ull_t c = power_mod(m, e, n);
        message_encrypted.push_back(c);
    }

    // decrypt message
    std::vector<char> message_decrypted;
    for (auto c : message_encrypted) {
        ull_t m = power_mod(c, d, n);
        message_decrypted.push_back(m);
    }

    std::cout << "n, e, d: ";
    std::cout << n << ", " << e << ", " << d << std::endl;

    for (auto c: message_decrypted) {
        std::cout << c << " ";
    }
    std::cout << std::endl;
}

void run_tests() {
    // tests factorize_N
    std::pair<ull_t,ull_t> pair;
    pair = factorize_n(100);
    assert((pow(2, pair.first) * pair.second + 1) == 100);

    pair = factorize_n(122);
    assert((pow(2, pair.first) * pair.second + 1) == 122);

    pair = factorize_n(3267);
    assert((pow(2, pair.first) * pair.second + 1) == 3267);

    // tests check_prime positive
    assert(check_prime(107, 10));
    assert(check_prime(193, 10));
    assert(check_prime(953, 10));
    assert(check_prime(4679, 10));
    assert(check_prime(9521, 10));

    assert(check_prime(100501, 10));
    assert(check_prime(117959, 10));
    assert(check_prime(126019, 10));
    assert(check_prime(149491, 10));
    assert(check_prime(192121, 10));

    assert(check_prime(141650963, 10));
    assert(check_prime(198491329, 10));
    assert(check_prime(735632791, 10));
    assert(check_prime(982451653, 10)); 

    // tests check_prime negative
    assert(!check_prime(15*3*7, 1));
    assert(!check_prime(123*3*7, 1));
    assert(!check_prime(3*3*7, 1));
    assert(!check_prime(13*43*312351, 1));
    assert(!check_prime(15*3*634565, 1));
    assert(!check_prime(15*3*1232333, 1));
    assert(!check_prime(15*11313111*7, 1));
    assert(!check_prime(13453617*3*7, 1));

    // gen prime
    assert(check_prime(gen_prime(5)));
    assert(check_prime(gen_prime(12)));
    assert(check_prime(gen_prime(24)));
    assert(check_prime(gen_prime(36)));
    assert(check_prime(gen_prime(61)));

    // gcd
    assert(gcd(421,111) == 1);
    assert(gcd(219,93) == 3);

    // gcd_extended
    {
    auto [d,x,y] = gcd_extended(23, 0);
    assert(d == 23);
    assert(x == 1);
    assert(y == 0);
    }

    {
    auto [d,x,y] = gcd_extended(421, 111);
    assert(d == 1);
    assert(x == -29);
    assert(y == 110);
    }

    {
    auto [d,x,y] = gcd_extended(111, 421);
    assert(d == 1);
    assert(x == 110);
    assert(y == -29);
    }

    // RSA
    {
    ull_t p, q, n, e, phi;
    p = 11;
    q = 3;
    n = p*q;
    e = 3;
    phi = (p-1)*(q-1);

    assert(gcd(e,p-1) == 1);

    auto [d,x,y] = gcd_extended(e, phi);
    assert(d == 1);
    assert(x == 7);
    assert((x*e-1) % phi == 0);
    }
}