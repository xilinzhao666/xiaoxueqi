#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>
#include "HospitalService.h"
#include "ApiHandler.h"

// 全局变量用于控制服务器状态
std::atomic<bool> serverRunning{false};
std::unique_ptr<std::thread> apiServerThread;

class IntegratedApiServer {
private:
    std::shared_ptr<ApiHandler> apiHandler;
    int serverSocket;
    std::vector<std::thread> clientThreads;
    
    void handleClient(int clientSocket) {
        char buffer[4096];
        
        while (serverRunning) {
            memset(buffer, 0, sizeof(buffer));
            ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            
            if (bytesReceived <= 0) {
                std::cout << "[API] 客户端断开连接" << std::endl;
                break;
            }
            
            std::string request(buffer, bytesReceived);
            std::cout << "[API] 收到请求: " << request << std::endl;
            
            // 处理API请求
            std::string response = apiHandler->processApiRequest(request);
            
            // 发送响应
            send(clientSocket, response.c_str(), response.length(), 0);
            std::cout << "[API] 发送响应: " << response << std::endl;
        }
        
        close(clientSocket);
    }
    
public:
    IntegratedApiServer(std::shared_ptr<HospitalService> hospitalService, int port = 8080) {
        apiHandler = std::make_shared<ApiHandler>(hospitalService);
        
        // 创建socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            throw std::runtime_error("创建socket失败");
        }
        
        // 设置socket选项
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        // 绑定地址
        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);
        
        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            close(serverSocket);
            throw std::runtime_error("绑定端口失败: " + std::to_string(port));
        }
        
        std::cout << "[API] API服务器初始化完成，端口: " << port << std::endl;
    }
    
    ~IntegratedApiServer() {
        stop();
    }
    
    void start() {
        if (listen(serverSocket, 10) < 0) {
            throw std::runtime_error("监听socket失败");
        }
        
        std::cout << "[API] API服务器启动，等待连接..." << std::endl;
        
        while (serverRunning) {
            sockaddr_in clientAddr{};
            socklen_t clientAddrLen = sizeof(clientAddr);
            
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
            if (clientSocket < 0) {
                if (serverRunning) {
                    std::cerr << "[API] 接受客户端连接失败" << std::endl;
                }
                continue;
            }
            
            std::cout << "[API] 新客户端连接" << std::endl;
            
            // 为每个客户端创建处理线程
            clientThreads.emplace_back(&IntegratedApiServer::handleClient, this, clientSocket);
        }
    }
    
    void stop() {
        serverRunning = false;
        
        if (serverSocket != -1) {
            close(serverSocket);
            serverSocket = -1;
        }
        
        // 等待所有客户端线程结束
        for (auto& thread : clientThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        clientThreads.clear();
        
        std::cout << "[API] API服务器已停止" << std::endl;
    }
    
    // 测试API功能
    void testApi() {
        std::cout << "\n=== API接口功能测试 ===" << std::endl;
        
        // 测试公共接口
        std::string testRequest1 = R"({"api": "public.schedule.list", "data": {}})";
        std::cout << "测试请求: " << testRequest1 << std::endl;
        std::cout << "响应: " << apiHandler->processApiRequest(testRequest1) << std::endl;
        
        // 测试患者注册
        std::string testRequest2 = R"({"api": "patient.auth.register", "data": {"email": "test@example.com", "password": "123456"}})";
        std::cout << "\n测试请求: " << testRequest2 << std::endl;
        std::cout << "响应: " << apiHandler->processApiRequest(testRequest2) << std::endl;
        
        // 测试医生登录
        std::string testRequest3 = R"({"api": "doctor.auth.login", "data": {"employeeId": "DOC1", "password": "123456"}})";
        std::cout << "\n测试请求: " << testRequest3 << std::endl;
        std::cout << "响应: " << apiHandler->processApiRequest(testRequest3) << std::endl;
        
        // 测试无效API
        std::string testRequest4 = R"({"api": "invalid.api", "data": {}})";
        std::cout << "\n测试请求: " << testRequest4 << std::endl;
        std::cout << "响应: " << apiHandler->processApiRequest(testRequest4) << std::endl;
    }
};

// 信号处理函数
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << "\n[系统] 收到退出信号，正在关闭服务器..." << std::endl;
        serverRunning = false;
    }
}

void printMenu() {
    std::cout << "\n=== 医院管理系统 ===" << std::endl;
    std::cout << "1. 初始化数据库" << std::endl;
    std::cout << "2. 注册用户" << std::endl;
    std::cout << "3. 用户登录" << std::endl;
    std::cout << "4. 注册医生信息" << std::endl;
    std::cout << "5. 注册患者信息" << std::endl;
    std::cout << "6. 创建病例" << std::endl;
    std::cout << "7. 预约挂号" << std::endl;
    std::cout << "8. 住院登记" << std::endl;
    std::cout << "9. 开具处方" << std::endl;
    std::cout << "10. 添加药物" << std::endl;
    std::cout << "11. 查看患者病例历史" << std::endl;
    std::cout << "12. 查看医生预约" << std::endl;
    std::cout << "13. 查看系统统计" << std::endl;
    std::cout << "14. 搜索功能" << std::endl;
    std::cout << "15. 启动API服务器" << std::endl;
    std::cout << "16. 停止API服务器" << std::endl;
    std::cout << "17. 测试API功能" << std::endl;
    std::cout << "0. 退出" << std::endl;
    std::cout << "请选择操作: ";
}

void printStats(const HospitalService::HospitalStats& stats) {
    std::cout << "\n=== 医院系统统计 ===" << std::endl;
    std::cout << "总用户数: " << stats.totalUsers << std::endl;
    std::cout << "医生数: " << stats.totalDoctors << std::endl;
    std::cout << "患者数: " << stats.totalPatients << std::endl;
    std::cout << "病例数: " << stats.totalCases << std::endl;
    std::cout << "总预约数: " << stats.totalAppointments << std::endl;
    std::cout << "已预约: " << stats.bookedAppointments << std::endl;
    std::cout << "已就诊: " << stats.attendedAppointments << std::endl;
    std::cout << "已取消: " << stats.cancelledAppointments << std::endl;
    std::cout << "住院记录数: " << stats.totalHospitalizations << std::endl;
    std::cout << "处方数: " << stats.totalPrescriptions << std::endl;
    std::cout << "药物记录数: " << stats.totalMedications << std::endl;
}

void printUser(const User& user) {
    std::cout << "用户ID: " << user.getUserId() << std::endl;
    std::cout << "用户名: " << user.getUsername() << std::endl;
    std::cout << "用户类型: " << user.userTypeToString() << std::endl;
    std::cout << "邮箱: " << user.getEmail() << std::endl;
    std::cout << "电话: " << user.getPhoneNumber() << std::endl;
    std::cout << "创建时间: " << user.getCreatedAt() << std::endl;
}

void printDoctor(const Doctor& doctor) {
    std::cout << "医生ID: " << doctor.getDoctorId() << std::endl;
    std::cout << "用户ID: " << doctor.getUserId() << std::endl;
    std::cout << "姓名: " << doctor.getName() << std::endl;
    std::cout << "科室: " << doctor.getDepartment() << std::endl;
    std::cout << "职称: " << doctor.getTitle() << std::endl;
    std::cout << "工作时间: " << doctor.getWorkingHours() << std::endl;
}

void printPatient(const Patient& patient) {
    std::cout << "患者ID: " << patient.getPatientId() << std::endl;
    std::cout << "用户ID: " << patient.getUserId() << std::endl;
    std::cout << "姓名: " << patient.getName() << std::endl;
    std::cout << "性别: " << patient.genderToString() << std::endl;
    std::cout << "出生日期: " << patient.getBirthDate() << std::endl;
    std::cout << "身份证号: " << patient.getIdNumber() << std::endl;
    std::cout << "电话: " << patient.getPhoneNumber() << std::endl;
    std::cout << "年龄: " << patient.getAge() << "岁" << std::endl;
}

void printCase(const Case& medicalCase) {
    std::cout << "病例ID: " << medicalCase.getCaseId() << std::endl;
    std::cout << "患者ID: " << medicalCase.getPatientId() << std::endl;
    std::cout << "科室: " << medicalCase.getDepartment() << std::endl;
    std::cout << "医生ID: " << medicalCase.getDoctorId() << std::endl;
    std::cout << "诊断: " << medicalCase.getDiagnosis() << std::endl;
    std::cout << "诊断日期: " << medicalCase.getDiagnosisDate() << std::endl;
}

void printAppointment(const Appointment& appointment) {
    std::cout << "预约ID: " << appointment.getAppointmentId() << std::endl;
    std::cout << "患者ID: " << appointment.getPatientId() << std::endl;
    std::cout << "医生ID: " << appointment.getDoctorId() << std::endl;
    std::cout << "预约时间: " << appointment.getAppointmentTime() << std::endl;
    std::cout << "科室: " << appointment.getDepartment() << std::endl;
    std::cout << "状态: " << appointment.statusToString() << std::endl;
}

int main() {
    std::cout << "=== C++ MySQL 医院管理系统 ===" << std::endl;
    
    // 设置信号处理
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    // Database connection configuration
    std::string host = "localhost";
    std::string username = "root";
    std::string password = "";
    std::string database = "hospital_db";
    
    std::cout << "请输入数据库连接信息:" << std::endl;
    std::cout << "主机 [" << host << "]: ";
    std::string input;
    std::getline(std::cin, input);
    if (!input.empty()) host = input;
    
    std::cout << "用户名 [" << username << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) username = input;
    
    std::cout << "密码: ";
    std::getline(std::cin, input);
    if (!input.empty()) password = input;
    
    std::cout << "数据库名 [" << database << "]: ";
    std::getline(std::cin, input);
    if (!input.empty()) database = input;
    
    try {
        // 初始化医院服务
        auto hospitalService = std::make_shared<HospitalService>(host, username, password, database);
        
        // 初始化API服务器（但不启动）
        std::unique_ptr<IntegratedApiServer> apiServer;
        
        int choice;
        while (true) {
            printMenu();
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 0: {
                    std::cout << "正在关闭系统..." << std::endl;
                    
                    // 停止API服务器
                    if (serverRunning) {
                        serverRunning = false;
                        if (apiServerThread && apiServerThread->joinable()) {
                            apiServerThread->join();
                        }
                    }
                    
                    std::cout << "感谢使用医院管理系统！再见！" << std::endl;
                    return 0;
                }
                
                case 1: {
                    std::cout << "正在初始化数据库..." << std::endl;
                    if (hospitalService->initializeDatabase()) {
                        std::cout << "数据库初始化成功！" << std::endl;
                    } else {
                        std::cout << "数据库初始化失败！" << std::endl;
                    }
                    break;
                }
                
                case 2: {
                    std::string username, password, email, phoneNumber;
                    int userTypeInt;
                    
                    std::cout << "用户名: ";
                    std::getline(std::cin, username);
                    std::cout << "密码: ";
                    std::getline(std::cin, password);
                    std::cout << "用户类型 (1-医生, 2-患者): ";
                    std::cin >> userTypeInt;
                    std::cin.ignore();
                    std::cout << "邮箱: ";
                    std::getline(std::cin, email);
                    std::cout << "电话: ";
                    std::getline(std::cin, phoneNumber);
                    
                    UserType userType = (userTypeInt == 1) ? UserType::DOCTOR : UserType::PATIENT;
                    
                    if (hospitalService->registerUser(username, password, userType, email, phoneNumber)) {
                        std::cout << "用户注册成功！" << std::endl;
                    } else {
                        std::cout << "用户注册失败！" << std::endl;
                    }
                    break;
                }
                
                case 3: {
                    std::string username, password;
                    std::cout << "用户名: ";
                    std::getline(std::cin, username);
                    std::cout << "密码: ";
                    std::getline(std::cin, password);
                    
                    auto user = hospitalService->loginUser(username, password);
                    if (user) {
                        std::cout << "登录成功！" << std::endl;
                        printUser(*user);
                    } else {
                        std::cout << "登录失败！用户名或密码错误。" << std::endl;
                    }
                    break;
                }
                
                case 4: {
                    int userId;
                    std::string name, department, workingHours, title;
                    
                    std::cout << "用户ID: ";
                    std::cin >> userId;
                    std::cin.ignore();
                    std::cout << "姓名: ";
                    std::getline(std::cin, name);
                    std::cout << "科室: ";
                    std::getline(std::cin, department);
                    std::cout << "工作时间: ";
                    std::getline(std::cin, workingHours);
                    std::cout << "职称: ";
                    std::getline(std::cin, title);
                    
                    if (hospitalService->registerDoctor(userId, name, department, workingHours, title)) {
                        std::cout << "医生信息注册成功！" << std::endl;
                    } else {
                        std::cout << "医生信息注册失败！" << std::endl;
                    }
                    break;
                }
                
                case 5: {
                    int userId, genderInt;
                    std::string name, birthDate, idNumber, phoneNumber;
                    
                    std::cout << "用户ID: ";
                    std::cin >> userId;
                    std::cin.ignore();
                    std::cout << "姓名: ";
                    std::getline(std::cin, name);
                    std::cout << "性别 (1-男, 2-女): ";
                    std::cin >> genderInt;
                    std::cin.ignore();
                    std::cout << "出生日期 (YYYY-MM-DD): ";
                    std::getline(std::cin, birthDate);
                    std::cout << "身份证号: ";
                    std::getline(std::cin, idNumber);
                    std::cout << "电话: ";
                    std::getline(std::cin, phoneNumber);
                    
                    Gender gender = (genderInt == 2) ? Gender::FEMALE : Gender::MALE;
                    
                    if (hospitalService->registerPatient(userId, name, gender, birthDate, idNumber, phoneNumber)) {
                        std::cout << "患者信息注册成功！" << std::endl;
                    } else {
                        std::cout << "患者信息注册失败！" << std::endl;
                    }
                    break;
                }
                
                case 6: {
                    int patientId, doctorId;
                    std::string department, diagnosis;
                    
                    std::cout << "患者ID: ";
                    std::cin >> patientId;
                    std::cin.ignore();
                    std::cout << "科室: ";
                    std::getline(std::cin, department);
                    std::cout << "医生ID: ";
                    std::cin >> doctorId;
                    std::cin.ignore();
                    std::cout << "诊断: ";
                    std::getline(std::cin, diagnosis);
                    
                    int caseId = hospitalService->createMedicalCase(patientId, department, doctorId, diagnosis);
                    if (caseId > 0) {
                        std::cout << "病例创建成功！病例ID: " << caseId << std::endl;
                    } else {
                        std::cout << "病例创建失败！" << std::endl;
                    }
                    break;
                }
                
                case 7: {
                    int patientId, doctorId;
                    std::string appointmentTime, department;
                    
                    std::cout << "患者ID: ";
                    std::cin >> patientId;
                    std::cin.ignore();
                    std::cout << "医生ID: ";
                    std::cin >> doctorId;
                    std::cin.ignore();
                    std::cout << "预约时间 (YYYY-MM-DD HH:MM:SS): ";
                    std::getline(std::cin, appointmentTime);
                    std::cout << "科室: ";
                    std::getline(std::cin, department);
                    
                    int appointmentId = hospitalService->bookAppointment(patientId, doctorId, appointmentTime, department);
                    if (appointmentId > 0) {
                        std::cout << "预约成功！预约ID: " << appointmentId << std::endl;
                    } else {
                        std::cout << "预约失败！" << std::endl;
                    }
                    break;
                }
                
                case 8: {
                    int patientId;
                    std::string wardNumber, bedNumber, attendingDoctor;
                    
                    std::cout << "患者ID: ";
                    std::cin >> patientId;
                    std::cin.ignore();
                    std::cout << "病房号: ";
                    std::getline(std::cin, wardNumber);
                    std::cout << "床位号: ";
                    std::getline(std::cin, bedNumber);
                    std::cout << "主治医生: ";
                    std::getline(std::cin, attendingDoctor);
                    
                    int hospitalizationId = hospitalService->admitPatient(patientId, wardNumber, bedNumber, attendingDoctor);
                    if (hospitalizationId > 0) {
                        std::cout << "住院登记成功！住院ID: " << hospitalizationId << std::endl;
                    } else {
                        std::cout << "住院登记失败！" << std::endl;
                    }
                    break;
                }
                
                case 9: {
                    int caseId, doctorId;
                    std::string prescriptionContent;
                    
                    std::cout << "病例ID: ";
                    std::cin >> caseId;
                    std::cin.ignore();
                    std::cout << "医生ID: ";
                    std::cin >> doctorId;
                    std::cin.ignore();
                    std::cout << "处方内容: ";
                    std::getline(std::cin, prescriptionContent);
                    
                    int prescriptionId = hospitalService->issuePrescription(caseId, doctorId, prescriptionContent);
                    if (prescriptionId > 0) {
                        std::cout << "处方开具成功！处方ID: " << prescriptionId << std::endl;
                    } else {
                        std::cout << "处方开具失败！" << std::endl;
                    }
                    break;
                }
                
                case 10: {
                    int prescriptionId, quantity;
                    std::string medicationName, usageInstructions;
                    
                    std::cout << "处方ID: ";
                    std::cin >> prescriptionId;
                    std::cin.ignore();
                    std::cout << "药物名称: ";
                    std::getline(std::cin, medicationName);
                    std::cout << "数量: ";
                    std::cin >> quantity;
                    std::cin.ignore();
                    std::cout << "用法说明: ";
                    std::getline(std::cin, usageInstructions);
                    
                    if (hospitalService->addMedication(prescriptionId, medicationName, quantity, usageInstructions)) {
                        std::cout << "药物添加成功！" << std::endl;
                    } else {
                        std::cout << "药物添加失败！" << std::endl;
                    }
                    break;
                }
                
                case 11: {
                    int patientId;
                    std::cout << "患者ID: ";
                    std::cin >> patientId;
                    
                    auto caseHistory = hospitalService->getPatientCaseHistory(patientId);
                    std::cout << "\n=== 患者病例历史 ===" << std::endl;
                    for (const auto& info : caseHistory) {
                        std::cout << "患者: " << info.patientName << " (ID: " << info.idNumber << ")" << std::endl;
                        std::cout << "病例ID: " << info.caseId << std::endl;
                        std::cout << "诊断: " << info.diagnosis << std::endl;
                        std::cout << "诊断日期: " << info.diagnosisDate << std::endl;
                        std::cout << "医生: " << info.doctorName << std::endl;
                        std::cout << "科室: " << info.department << std::endl;
                        std::cout << "---" << std::endl;
                    }
                    break;
                }
                
                case 12: {
                    int doctorId;
                    std::cout << "医生ID: ";
                    std::cin >> doctorId;
                    
                    auto appointments = hospitalService->getDoctorAppointments(doctorId);
                    std::cout << "\n=== 医生预约列表 ===" << std::endl;
                    for (const auto& info : appointments) {
                        std::cout << "医生: " << info.doctorName << " (" << info.department << ")" << std::endl;
                        std::cout << "预约ID: " << info.appointmentId << std::endl;
                        std::cout << "患者: " << info.patientName << std::endl;
                        std::cout << "预约时间: " << info.appointmentTime << std::endl;
                        std::cout << "状态: " << info.status << std::endl;
                        std::cout << "---" << std::endl;
                    }
                    break;
                }
                
                case 13: {
                    auto stats = hospitalService->getHospitalStats();
                    printStats(stats);
                    break;
                }
                
                case 14: {
                    int searchType;
                    std::string searchTerm;
                    
                    std::cout << "搜索类型 (1-用户, 2-医生, 3-患者, 4-病例): ";
                    std::cin >> searchType;
                    std::cin.ignore();
                    std::cout << "搜索关键词: ";
                    std::getline(std::cin, searchTerm);
                    
                    switch (searchType) {
                        case 1: {
                            auto users = hospitalService->getUserDAO()->searchUsers(searchTerm);
                            std::cout << "\n=== 用户搜索结果 ===" << std::endl;
                            for (const auto& user : users) {
                                printUser(*user);
                                std::cout << "---" << std::endl;
                            }
                            break;
                        }
                        case 2: {
                            auto doctors = hospitalService->getDoctorDAO()->searchDoctors(searchTerm);
                            std::cout << "\n=== 医生搜索结果 ===" << std::endl;
                            for (const auto& doctor : doctors) {
                                printDoctor(*doctor);
                                std::cout << "---" << std::endl;
                            }
                            break;
                        }
                        case 3: {
                            auto patients = hospitalService->getPatientDAO()->searchPatients(searchTerm);
                            std::cout << "\n=== 患者搜索结果 ===" << std::endl;
                            for (const auto& patient : patients) {
                                printPatient(*patient);
                                std::cout << "---" << std::endl;
                            }
                            break;
                        }
                        case 4: {
                            auto cases = hospitalService->getCaseDAO()->searchCases(searchTerm);
                            std::cout << "\n=== 病例搜索结果 ===" << std::endl;
                            for (const auto& medicalCase : cases) {
                                printCase(*medicalCase);
                                std::cout << "---" << std::endl;
                            }
                            break;
                        }
                        default:
                            std::cout << "无效的搜索类型！" << std::endl;
                            break;
                    }
                    break;
                }
                
                case 15: {
                    if (serverRunning) {
                        std::cout << "API服务器已在运行中！" << std::endl;
                        break;
                    }
                    
                    try {
                        apiServer = std::make_unique<IntegratedApiServer>(hospitalService, 8080);
                        serverRunning = true;
                        
                        // 在新线程中启动API服务器
                        apiServerThread = std::make_unique<std::thread>([&apiServer]() {
                            try {
                                apiServer->start();
                            } catch (const std::exception& e) {
                                std::cerr << "[API] 服务器启动失败: " << e.what() << std::endl;
                                serverRunning = false;
                            }
                        });
                        
                        std::cout << "API服务器启动成功！端口: 8080" << std::endl;
                        std::cout << "可以使用以下命令测试:" << std::endl;
                        std::cout << "echo '{\"api\": \"public.schedule.list\", \"data\": {}}' | nc localhost 8080" << std::endl;
                        
                    } catch (const std::exception& e) {
                        std::cerr << "API服务器启动失败: " << e.what() << std::endl;
                        serverRunning = false;
                    }
                    break;
                }
                
                case 16: {
                    if (!serverRunning) {
                        std::cout << "API服务器未运行！" << std::endl;
                        break;
                    }
                    
                    std::cout << "正在停止API服务器..." << std::endl;
                    serverRunning = false;
                    
                    if (apiServerThread && apiServerThread->joinable()) {
                        apiServerThread->join();
                    }
                    
                    apiServer.reset();
                    std::cout << "API服务器已停止！" << std::endl;
                    break;
                }
                
                case 17: {
                    if (!apiServer) {
                        apiServer = std::make_unique<IntegratedApiServer>(hospitalService, 8080);
                    }
                    
                    std::cout << "正在测试API功能..." << std::endl;
                    apiServer->testApi();
                    break;
                }
                
                default: {
                    std::cout << "无效选择，请重试。" << std::endl;
                    break;
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}