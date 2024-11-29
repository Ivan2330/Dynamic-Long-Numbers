#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <chrono>

class BInt {
private:
    std::vector<int> numeric;
    bool is_negative;

public:
    BInt(const std::string& number) {
        if (number.empty()) {
            throw std::invalid_argument("Invalid number: empty string");
        }
        size_t start_idx = 0;
        is_negative = false;

        if (number[0] == '-') {
            is_negative = true;
            start_idx = 1;
        }

        for (size_t i = start_idx; i < number.size(); ++i) {
            if (number[i] < '0' || number[i] > '9') {
                throw std::invalid_argument("Invalid character in number");
            }
            numeric.push_back(number[i] - '0');
        }

        std::reverse(numeric.begin(), numeric.end());
        remove_zeros();
    }

    BInt() : numeric{0}, is_negative(false) {}

    BInt operator+(const BInt& other) const {
        if (is_negative == other.is_negative) {
            BInt result = add_absolute(other);
            result.is_negative = is_negative;
            return result;
        }

        if (absIsLessThan(other)) {
            BInt result = other.subtract_absolute(*this);
            result.is_negative = other.is_negative;
            return result;
        }

        BInt result = subtract_absolute(other);
        result.is_negative = is_negative;
        return result;
    }

    BInt operator-(const BInt& other) const {
        if (is_negative != other.is_negative) {
            BInt result = add_absolute(other);
            result.is_negative = is_negative;
            return result;
        }

        if (absIsLessThan(other)) {
            BInt result = other.subtract_absolute(*this);
            result.is_negative = !is_negative;
            return result;
        }

        BInt result = subtract_absolute(other);
        result.is_negative = is_negative;
        return result;
    }

    BInt operator*(const BInt& other) const {
        BInt result;
        result.numeric.resize(numeric.size() + other.numeric.size(), 0);

        for (size_t i = 0; i < numeric.size(); ++i) {
            int carry = 0;
            for (size_t j = 0; j < other.numeric.size() || carry; ++j) {
                long long current = result.numeric[i + j] +
                                    numeric[i] * (j < other.numeric.size() ? other.numeric[j] : 0) + carry;
                result.numeric[i + j] = current % 10;
                carry = current / 10;
            }
        }

        result.remove_zeros();
        result.is_negative = (is_negative != other.is_negative) && !(result.numeric.size() == 1 && result.numeric[0] == 0);
        return result;
    }

    BInt operator/(const BInt& other) const {
        if (other == BInt("0")) {
            throw std::invalid_argument("Cannot divide by zero");
        }

        BInt dividend = *this;
        dividend.is_negative = false;

        BInt divisor = other;
        divisor.is_negative = false;

        BInt quotient, remainder;

        for (size_t i = dividend.numeric.size(); i-- > 0;) {
            remainder = remainder * BInt("10") + BInt(std::to_string(dividend.numeric[i]));
            int count = 0;
            while (!remainder.absIsLessThan(divisor)) {
                remainder = remainder - divisor;
                ++count;
            }
            quotient.numeric.push_back(count);
        }

        std::reverse(quotient.numeric.begin(), quotient.numeric.end());
        quotient.remove_zeros();
        quotient.is_negative = (is_negative != other.is_negative);

        return quotient;
    }

BInt get_sub_bint(int start, int end) const {
    std::vector<int> sub_numeric;
    for (int i = start; i < end && i < numeric.size(); ++i) {
        sub_numeric.push_back(numeric[i]);
    }
    BInt result;
    result.numeric = sub_numeric;
    result.is_negative = false;
    result.remove_zeros();
    return result;
}


static BInt karatsuba(const BInt& x, const BInt& y) {
    if (x == BInt("0") || y == BInt("0")) {
        return BInt("0");
    }

    
    if (x.numeric.size() == 1 || y.numeric.size() == 1) {
        return x * y;
    }

    int n = std::max(x.numeric.size(), y.numeric.size());
    int half = n / 2;

    
    BInt high1 = x.get_sub_bint(half, x.numeric.size());
    BInt low1 = x.get_sub_bint(0, half);
    BInt high2 = y.get_sub_bint(half, y.numeric.size());
    BInt low2 = y.get_sub_bint(0, half);

    
    BInt P1 = karatsuba(high1, high2);
    BInt P2 = karatsuba(low1, low2);
    BInt P3 = karatsuba(high1 + low1, high2 + low2) - P1 - P2;

    
    BInt result = P1.shift_left(2 * half) + P3.shift_left(half) + P2;

    
    result.is_negative = (x.is_negative != y.is_negative);

    return result;
}



    BInt operator%(const BInt& other) const {
        if (other == BInt("0")) {
            throw std::invalid_argument("Cannot divide by zero");
        }

        BInt dividend = *this;
        dividend.is_negative = false;

        BInt divisor = other;
        divisor.is_negative = false;

        BInt remainder;

        for (size_t i = dividend.numeric.size(); i-- > 0;) {
            remainder = remainder * BInt("10") + BInt(std::to_string(dividend.numeric[i]));
            while (!remainder.absIsLessThan(divisor)) {
                remainder = remainder - divisor;
            }
        }

        remainder.is_negative = is_negative;
        return remainder;
    }

    bool operator==(const BInt& other) const {
        return is_negative == other.is_negative && numeric == other.numeric;
    }

    bool operator<(const BInt& other) const {
        if (is_negative != other.is_negative) {
            return is_negative;
        }

        if (numeric.size() != other.numeric.size()) {
            return numeric.size() < other.numeric.size();
        }

        for (size_t i = numeric.size(); i-- > 0;) {
            if (numeric[i] != other.numeric[i]) {
                return numeric[i] < other.numeric[i];
            }
        }

        return false;
    }

    BInt abs() const {
        BInt result = *this;
        result.is_negative = false;
        return result;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const BInt& value) {
        if (value.is_negative && !(value.numeric.size() == 1 && value.numeric[0] == 0)) {
            os << '-';
        }
        for (auto it = value.numeric.rbegin(); it != value.numeric.rend(); ++it) {
            os << *it;
        }
        return os;
    }

private:
    void remove_zeros() {
        while (numeric.size() > 1 && numeric.back() == 0) {
            numeric.pop_back();
        }
        if (numeric.size() == 1 && numeric[0] == 0) {
            is_negative = false;
        }
    }

    BInt add_absolute(const BInt& other) const {
        BInt result;
        result.numeric.clear();

        int carry = 0;
        size_t max_size = std::max(numeric.size(), other.numeric.size());

        for (size_t i = 0; i < max_size || carry; ++i) {
            int digit1 = (i < numeric.size()) ? numeric[i] : 0;
            int digit2 = (i < other.numeric.size()) ? other.numeric[i] : 0;

            int sum = digit1 + digit2 + carry;
            result.numeric.push_back(sum % 10);
            carry = sum / 10;
        }

        return result;
    }

    BInt subtract_absolute(const BInt& other) const {
        BInt result;
        result.numeric.clear();

        int borrow = 0;
        for (size_t i = 0; i < numeric.size(); ++i) {
            int digit1 = numeric[i];
            int digit2 = (i < other.numeric.size()) ? other.numeric[i] : 0;

            int diff = digit1 - digit2 - borrow;
            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }

            result.numeric.push_back(diff);
        }

        result.remove_zeros();
        return result;
    }

    bool absIsLessThan(const BInt& other) const {
        if (numeric.size() != other.numeric.size()) {
            return numeric.size() < other.numeric.size();
        }

        for (size_t i = numeric.size(); i-- > 0;) {
            if (numeric[i] != other.numeric[i]) {
                return numeric[i] < other.numeric[i];
            }
        }

        return false;
    }

    std::string to_string() const {
    std::string result;
    if (is_negative) result += '-';
    for (auto it = numeric.rbegin(); it != numeric.rend(); ++it) {
        result += std::to_string(*it);
    }
    return result;
}
    
    BInt shift_left(int positions) const {
        if (*this == BInt("0")) return *this;
        BInt result = *this;
        result.numeric.insert(result.numeric.begin(), positions, 0);
        return result;
    }
};

template <typename T>
class RationalTemplate {
private:
    T numerator;
    T denominator;

    void simplify() {
        T gcd = compute_gcd(numerator.abs(), denominator.abs());
        numerator = numerator / gcd;
        denominator = denominator / gcd;

        if (denominator < T("0")) {
            numerator = numerator * T("-1");
            denominator = denominator * T("-1");
        }
    }

    T compute_gcd(const T& a, const T& b) const {
        return (b == T("0")) ? a : compute_gcd(b, a % b);
    }

public:
    RationalTemplate(T num, T den) {
        if (den == T("0")) {
            throw std::invalid_argument("Denominator cannot be zero.");
        }
        numerator = num;
        denominator = den;
        simplify();
    }

    RationalTemplate operator+(const RationalTemplate& other) const {
        T num = numerator * other.denominator + other.numerator * denominator;
        T den = denominator * other.denominator;
        return RationalTemplate(num, den);
    }

    RationalTemplate operator-(const RationalTemplate& other) const {
        T num = numerator * other.denominator - other.numerator * denominator;
        T den = denominator * other.denominator;
        return RationalTemplate(num, den);
    }

    RationalTemplate operator*(const RationalTemplate& other) const {
        T num = numerator * other.numerator;
        T den = denominator * other.denominator;
        return RationalTemplate(num, den);
    }

    RationalTemplate operator/(const RationalTemplate& other) const {
        if (other.numerator == T("0")) {
            throw std::invalid_argument("Cannot divide by zero.");
        }
        T num = numerator * other.denominator;
        T den = denominator * other.numerator;
        return RationalTemplate(num, den);
    }

    void print() const {
        std::cout << numerator << "/" << denominator << std::endl;
    }
};


class Rational {
private:
    int numerator;
    int denominator;

    void simplify() {
        int gcd = std::gcd(numerator, denominator);
        numerator /= gcd;
        denominator /= gcd;

        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
    }

public:
    Rational(int num, int den) {
        if (den == 0) {
            throw std::invalid_argument("Denominator cannot be zero.");
        }
        numerator = num;
        denominator = den;
        simplify();
    }

    Rational operator+(const Rational& other) const {
        int num = numerator * other.denominator + other.numerator * denominator;
        int den = denominator * other.denominator;
        return Rational(num, den);
    }

    Rational operator-(const Rational& other) const {
        int num = numerator * other.denominator - other.numerator * denominator;
        int den = denominator * other.denominator;
        return Rational(num, den);
    }
                                                                                               
    Rational operator*(const Rational& other) const {
        return Rational(numerator * other.numerator, denominator * other.denominator);
    }

    Rational operator/(const Rational& other) const {
        if (other.numerator == 0) {
            throw std::invalid_argument("Cannot divide by zero.");
        }
        return Rational(numerator * other.denominator, denominator * other.numerator);
    }

    void print() const {
        std::cout << numerator << "/" << denominator << std::endl;
    }
};

int main() {
    using namespace std::chrono;

    // BInt тести
    BInt num1("36423452523432434");
    BInt num2("-18423432424542345");

    std::cout << "BInt арифметика:" << std::endl;

    auto start = high_resolution_clock::now();
    BInt sum = num1 + num2;
    auto end = high_resolution_clock::now();
    std::cout << "Сума: " << sum << " (Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    start = high_resolution_clock::now();
    BInt diff = num1 - num2;
    end = high_resolution_clock::now();
    std::cout << "Різниця: " << diff << " (Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    start = high_resolution_clock::now();
    BInt prod_naive = num1 * num2;
    end = high_resolution_clock::now();
    std::cout << "Добуток (наївне): " << prod_naive << " (Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    start = high_resolution_clock::now();
    BInt prod_karatsuba = BInt::karatsuba(num1, num2);
    end = high_resolution_clock::now();
    std::cout << "Добуток (Карацуба): " << prod_karatsuba << " (Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    start = high_resolution_clock::now();
    BInt quot = num1 / num2;
    end = high_resolution_clock::now();
    std::cout << "Частка: " << quot << " (Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    // RationalTemplate тести
    std::cout << "\nRationalTemplate арифметика:" << std::endl;
    RationalTemplate<BInt> rt1(BInt("3"), BInt("4"));
    RationalTemplate<BInt> rt2(BInt("5"), BInt("6"));

    start = high_resolution_clock::now();
    RationalTemplate<BInt> rtSum = rt1 + rt2;
    end = high_resolution_clock::now();
    std::cout << "Сума дробів: ";
    rtSum.print();
    std::cout << "(Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    start = high_resolution_clock::now();
    RationalTemplate<BInt> rtDiff = rt1 - rt2;
    end = high_resolution_clock::now();
    std::cout << "Різниця дробів: ";
    rtDiff.print();
    std::cout << "(Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    start = high_resolution_clock::now();
    RationalTemplate<BInt> rtProd = rt1 * rt2;
    end = high_resolution_clock::now();
    std::cout << "Добуток дробів: ";
    rtProd.print();
    std::cout << "(Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    start = high_resolution_clock::now();
    RationalTemplate<BInt> rtQuot = rt1 / rt2;
    end = high_resolution_clock::now();
    std::cout << "Частка дробів: ";
    rtQuot.print();
    std::cout << "(Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    // Rational тести
    std::cout << "\nRational арифметика:" << std::endl;
    Rational r1(3, 4);
    Rational r2(5, 6);

    start = high_resolution_clock::now();
    Rational rSum = r1 + r2;
    end = high_resolution_clock::now();
    std::cout << "Сума дробів: ";
    rSum.print();
    std::cout << "(Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    start = high_resolution_clock::now();
    Rational rDiff = r1 - r2;
    end = high_resolution_clock::now();
    std::cout << "Різниця дробів: ";
    rDiff.print();
    std::cout << "(Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    start = high_resolution_clock::now();
    Rational rProd = r1 * r2;
    end = high_resolution_clock::now();
    std::cout << "Добуток дробів: ";
    rProd.print();
    std::cout << "(Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    start = high_resolution_clock::now();
    Rational rQuot = r1 / r2;
    end = high_resolution_clock::now();
    std::cout << "Частка дробів: ";
    rQuot.print();
    std::cout << "(Час: " << duration_cast<nanoseconds>(end - start).count() << " нс)\n";

    return 0;
}

