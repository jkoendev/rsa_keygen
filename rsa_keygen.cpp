#include <random>
#include <cmath>
#include <cassert>
#include <iostream>

void run_tests();
typedef unsigned long long ull_t;


// find x,y for gcd(a,b)=xa+yb
void gcd_extended(ull_t a, ull_t b, ull_t &out_gcd, ull_t &out_x, ull_t &out_y) {

    assert(a>0);
    assert(b>0);

    assert(a>b);

    if (b==0) {
        out_gcd = a;
        out_x = 1;
        out_y = 0;
        return;
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
        x2 = y1;
        y1 = y;
    }

    out_gcd = a;
    out_x = x2;
    out_y = y2;

    assert(a==1);
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

// Calculates a^n % p 
ull_t power_mod(ull_t a, ull_t n, ull_t p) {
    
    ull_t res = 1; 
    a = a % p;   
  
    while (n > 0) { 
        if (n & 1) {
            res = (res*a) % p;
        } 
        n = n>>1; 
        a = (a*a) % p; 
        
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
    std::uniform_int_distribution<ull_t> dist(2<<bit_length, (2<<bit_length+1)-1); 
    ull_t x  = dist(rng) | 1;

    while (!check_prime(x, num_prime_tests)) {
        x += 2;
    }
    return x;
}

int main() {

    run_tests();

    int num_prime_tests = 10;

    int k = 16;
    int e = 17; // 65537;

    ull_t p;
    do {
        p = gen_prime(k/2, num_prime_tests);
    } while (p%e != 1);

    ull_t q;
    do {
        q = gen_prime(k-k/2, num_prime_tests);
    } while(q%e != 1);

    ull_t N = p*q;
    ull_t L = (p-1)*(q-1);
    ull_t d = (1/e) % L;

    std::cout << N << ", " << L << ", " << d << std::endl;
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
    ull_t out_gcd;
    ull_t out_x;
    ull_t out_y;
    gcd_extended(421, 111, out_gcd, out_x, out_y);
    assert(out_gcd == 1);
    assert(out_x == -29);
    assert(out_y == 110);


}