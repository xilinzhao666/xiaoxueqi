#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <getopt.h>
#include "HospitalService.h"
#include "ApiHandler.h"

void printUsage(const char* programName) {
    std::cout << "用法: " << programName << " [选项]" << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  --input <文件路径>    指定输入JSON请求文件路径" << std::endl;
    std::cout << "  --output <文件路径>   指定输出JSON响应文件路径" << std::endl;
    std::cout << "  --host <主机地址>     数据库主机地址 (默认: localhost)" << std::endl;
    std::cout << "  --user <用户名>       数据库用户名 (默认: root)" << std::endl;
    std::cout << "  --password <密码>     数据库密码 (默认: 空)" << std::endl;
    std::cout << "  --database <数据库名> 数据库名称 (默认: hospital_db)" << std::endl;
    std::cout << "  --help               显示此帮助信息" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  " << programName << " --input request.json --output response.json" << std::endl;
    std::cout << "  " << programName << " --input test.json --output result.json --host 192.168.1.100 --user admin" << std::endl;
}

std::string readFileContent(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开输入文件: " + filePath);
    }
    
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    file.close();
    return content;
}

void writeFileContent(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("无法创建输出文件: " + filePath);
    }
    
    file << content;
    file.close();
}

int main(int argc, char* argv[]) {
    std::cout << "=== 医院管理系统 JSON API 处理器 ===" << std::endl;
    
    // 命令行参数
    std::string inputFile;
    std::string outputFile;
    std::string host = "localhost";
    std::string username = "root";
    std::string password = "";
    std::string database = "hospital_db";
    
    // 解析命令行参数
    static struct option long_options[] = {
        {"input",    required_argument, 0, 'i'},
        {"output",   required_argument, 0, 'o'},
        {"host",     required_argument, 0, 'h'},
        {"user",     required_argument, 0, 'u'},
        {"password", required_argument, 0, 'p'},
        {"database", required_argument, 0, 'd'},
        {"help",     no_argument,       0, '?'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "i:o:h:u:p:d:?", long_options, &option_index)) != -1) {
        switch (c) {
            case 'i':
                inputFile = optarg;
                break;
            case 'o':
                outputFile = optarg;
                break;
            case 'h':
                host = optarg;
                break;
            case 'u':
                username = optarg;
                break;
            case 'p':
                password = optarg;
                break;
            case 'd':
                database = optarg;
                break;
            case '?':
            default:
                printUsage(argv[0]);
                return 0;
        }
    }
    
    // 验证必需参数
    if (inputFile.empty() || outputFile.empty()) {
        std::cerr << "错误: 必须指定输入文件和输出文件路径" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        std::cout << "连接数据库: " << host << "/" << database << " (用户: " << username << ")" << std::endl;
        
        // 初始化医院服务
        auto hospitalService = std::make_shared<HospitalService>(host, username, password, database);
        
        // 初始化API处理器
        auto apiHandler = std::make_shared<ApiHandler>(hospitalService);
        
        std::cout << "读取输入文件: " << inputFile << std::endl;
        
        // 读取输入JSON文件
        std::string jsonInput = readFileContent(inputFile);
        
        std::cout << "处理JSON请求..." << std::endl;
        std::cout << "请求内容: " << jsonInput << std::endl;
        
        // 处理API请求
        std::string jsonResponse = apiHandler->processApiRequest(jsonInput);
        
        std::cout << "响应内容: " << jsonResponse << std::endl;
        std::cout << "写入输出文件: " << outputFile << std::endl;
        
        // 写入输出JSON文件
        writeFileContent(outputFile, jsonResponse);
        
        std::cout << "JSON API处理完成！" << std::endl;
        
        // 显示处理结果摘要
        std::cout << "\n=== 处理结果摘要 ===" << std::endl;
        std::cout << "输入文件: " << inputFile << std::endl;
        std::cout << "输出文件: " << outputFile << std::endl;
        std::cout << "请求大小: " << jsonInput.length() << " 字节" << std::endl;
        std::cout << "响应大小: " << jsonResponse.length() << " 字节" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "处理失败: " << e.what() << std::endl;
        
        // 写入错误响应到输出文件
        try {
            std::string errorResponse = R"({"status": "error", "code": 500, "message": ")" + std::string(e.what()) + R"(", "data": {}})";
            writeFileContent(outputFile, errorResponse);
            std::cout << "错误信息已写入输出文件: " << outputFile << std::endl;
        } catch (const std::exception& writeError) {
            std::cerr << "无法写入错误信息到输出文件: " << writeError.what() << std::endl;
        }
        
        return 1;
    }
    
    return 0;
}