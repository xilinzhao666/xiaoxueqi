#!/usr/bin/env node

const { execSync } = require('child_process');
const fs = require('fs');
const path = require('path');

// Create necessary directories
const dirs = ['obj', 'bin'];
dirs.forEach(dir => {
  if (!fs.existsSync(dir)) {
    fs.mkdirSync(dir, { recursive: true });
    console.log(`Created directory: ${dir}`);
  }
});

// Compiler settings
const CXX = 'g++';
const CXXFLAGS = '-std=c++17 -Wall -Wextra -I./include -I/usr/include/mysql';
const LDFLAGS = '-lmysqlclient -lssl -lcrypto';

// Find all .cpp files in src directory
const srcDir = './src';
const cppFiles = fs.readdirSync(srcDir)
  .filter(file => file.endsWith('.cpp'))
  .map(file => path.join(srcDir, file));

if (cppFiles.length === 0) {
  console.error('No .cpp files found in src directory');
  process.exit(1);
}

console.log('Found C++ source files:');
cppFiles.forEach(file => console.log(`  ${file}`));

try {
  // Compile all source files
  const sourceFiles = cppFiles.join(' ');
  const command = `${CXX} ${CXXFLAGS} ${sourceFiles} -o bin/HospitalSystem ${LDFLAGS}`;
  
  console.log('\nCompiling...');
  console.log(`Command: ${command}`);
  
  execSync(command, { stdio: 'inherit' });
  
  console.log('\n✅ Compilation successful!');
  console.log('Executable created: bin/HospitalSystem');
  
} catch (error) {
  console.error('\n❌ Compilation failed:');
  console.error(error.message);
  process.exit(1);
}