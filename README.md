# Image Steganography in C (LSB Technique)

A command-line based Image Steganography project developed in C that securely hides and extracts secret data inside BMP image files using the Least Significant Bit (LSB) technique. This project demonstrates low-level file handling, bitwise manipulation, and image processing concepts.

---

## 📌 Key Features

- Encode secret text into BMP images  
- Decode hidden data from stego images  
- Uses LSB (Least Significant Bit) technique  
- Command-line based execution  
- Robust error handling  
- Efficient file I/O operations  
- Modular programming using header files  
- Built using Makefile  

---

## 🛠 Technologies & Concepts

- C Programming  
- File Handling  
- Bitwise Operations  
- BMP Image Processing  
- Makefile Automation  
- Modular Code Design  

---

## 📂 Project Structure

- encode.c  
- decode.c  
- encode.h  
- decode.h  
- common.h  
- types.h  
- Makefile  
- beautiful.bmp (sample input image)  

---

## ⚙️ Build Instructions

Ensure `make` is installed, then run:

```bash
make
```

This will generate the executable.

To clean build files:

```bash
make clean
```

---

## ▶️ Usage

### 🔹 Encoding Secret Data

```bash
./stego -e beautiful.bmp secret.txt stego.bmp
```

### 🔹 Decoding Hidden Data

```bash
./stego -d stego.bmp decoded.txt
```

*(Replace `stego` with your executable name if different.)*

---

## 🖥 Sample Output

### Encoding

```
INFO: ## Encoding Procedure Started ##
INFO: Opening required files
INFO: Encoding Magic String Signature
INFO: Encoding secret.txt File Data
INFO: ## Encoding Done Successfully ##
Encoding is successful!
```

### Decoding

```
INFO: ## Decoding Procedure Started ##
INFO: Magic string verified successfully
INFO: Decoding File Data
INFO: ## Decoding Done Successfully ##
Decoding is successful!
```

---

## 🧠 Algorithm Overview

- Least Significant Bit (LSB) Steganography  
- Bitwise manipulation to embed secret data  
- Preservation of BMP header information  
- Sequential file pointer operations  
- Magic string validation for secure decoding  

---

## 🎯 Learning Outcomes

- Practical understanding of steganography  
- Strong hands-on experience with C file I/O  
- Bit-level data manipulation  
- Working with binary image formats  
- Using Makefile for project automation  
- Writing modular and maintainable C code  

---

## 📜 License

This project is licensed under the MIT License.

---

## 👨‍💻 Author

Prasad Kumbhar
