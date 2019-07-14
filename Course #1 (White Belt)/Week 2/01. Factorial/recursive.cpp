int Factorial(int x) {
    if (x <= 1) {
        return 1;
    } else {
        return x * Factorial(x - 1);
    }
}
