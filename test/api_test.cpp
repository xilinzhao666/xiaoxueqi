#include <iostream>
#include <cassert>
#include <memory>
#include "../include/ApiHandler.h"
#include "../include/HospitalService.h"

class ApiTester {
private:
    std::shared_ptr<ApiHandler> apiHandler;
    
public:
    ApiTester(std::shared_ptr<HospitalService> hospitalService) {
        apiHandler = std::make_shared<ApiHandler>(hospitalService);
    }
    
    void runAllTests() {
        std::cout << "=== API接口测试开始 ===" << std::endl;
        
        testPublicApis();
        testPatientApis();
        testDoctorApis();
        testErrorHandling();
        
        std::cout << "=== 所有测试完成 ===" << std::endl;
    }
    
private:
    void testPublicApis() {
        std::cout << "\n--- 测试公共接口 ---" << std::endl;
        
        // 测试获取医生排班
        std::string request = R"({"api": "public.schedule.list", "data": {}})";
        std::string response = apiHandler->processApiRequest(request);
        std::cout << "排班查询: " << response << std::endl;
        
        // 测试获取医生详情
        request = R"({"api": "public.doctor.get", "data": {"doctorId": "doc_1"}})";
        response = apiHandler->processApiRequest(request);
        std::cout << "医生详情: " << response << std::endl;
    }
    
    void testPatientApis() {
        std::cout << "\n--- 测试患者接口 ---" << std::endl;
        
        // 测试患者注册
        std::string request = R"({"api": "patient.auth.register", "data": {"email": "test_patient@example.com", "password": "123456"}})";
        std::string response = apiHandler->processApiRequest(request);
        std::cout << "患者注册: " << response << std::endl;
        
        // 测试患者登录
        request = R"({"api": "patient.auth.login", "data": {"account": "test_patient@example.com", "password": "123456"}})";
        response = apiHandler->processApiRequest(request);
        std::cout << "患者登录: " << response << std::endl;
        
        // 解析登录响应获取token（简化处理）
        if (response.find("success") != std::string::npos) {
            // 在实际实现中，应该解析JSON获取token
            std::string token = "test_token_123";
            
            // 测试获取个人信息
            request = R"({"api": "patient.profile.get", "data": {"token": ")" + token + R"("}})";
            response = apiHandler->processApiRequest(request);
            std::cout << "获取个人信息: " << response << std::endl;
        }
    }
    
    void testDoctorApis() {
        std::cout << "\n--- 测试医生接口 ---" << std::endl;
        
        // 测试医生登录
        std::string request = R"({"api": "doctor.auth.login", "data": {"employeeId": "DOC1", "password": "123456"}})";
        std::string response = apiHandler->processApiRequest(request);
        std::cout << "医生登录: " << response << std::endl;
    }
    
    void testErrorHandling() {
        std::cout << "\n--- 测试错误处理 ---" << std::endl;
        
        // 测试无效JSON
        std::string response = apiHandler->processApiRequest("invalid json");
        std::cout << "无效JSON: " << response << std::endl;
        
        // 测试缺少字段
        response = apiHandler->processApiRequest(R"({"api": "test"})");
        std::cout << "缺少字段: " << response << std::endl;
        
        // 测试不存在的API
        response = apiHandler->processApiRequest(R"({"api": "nonexistent.api", "data": {}})");
        std::cout << "不存在的API: " << response << std::endl;
    }
};

int main() {
    try {
        // 初始化医院服务
        auto hospitalService = std::make_shared<HospitalService>(
            "localhost", "root", "", "hospital_db");
        
        if (!hospitalService->initializeDatabase()) {
            std::cerr << "Failed to initialize database" << std::endl;
            return 1;
        }
        
        // 运行API测试
        ApiTester tester(hospitalService);
        tester.runAllTests();
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}