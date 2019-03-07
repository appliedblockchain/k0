TEST(SHA256Compression, EqualityOfImplementations) {
unsigned char input[64];
unsigned char output[32];
for (int i = 0; i < 64; i++) {
input[i] = 0;
}
for (int i = 0; i < 32; i++) {
cout << int(input[i]) << " ";
}
cout << endl;
sha256_compress(input, output);
cout << hex;
for (int i = 0; i < 32; i++) {
cout << int(output[i]) << " ";
}
cout << endl;
};

}

int main(int argc, char **argv) {
return RUN_ALL_TESTS();
}

