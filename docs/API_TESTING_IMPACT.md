# API测试对数据库的影响说明

## 🤔 **什么是"API测试中动态创建新用户"？**

### **问题解释**

当您运行 `sql/api_test_examples.sql` 脚本时，这个脚本会：

1. **动态创建新用户**：
   ```sql
   -- 在测试过程中创建新医生用户
   INSERT INTO users (username, password, user_type, email, phone_number) 
   VALUES ('dr_new', SHA2('newpassword123', 256), 'Doctor', 'dr_new@hospital.com', '13800138999');
   
   -- 在测试过程中创建新患者用户
   INSERT INTO users (username, password, user_type, email, phone_number) 
   VALUES ('patient_new', SHA2('newpassword123', 256), 'Patient', 'patient_new@email.com', '13900139999');
   ```

2. **为新用户添加详细信息**：
   ```sql
   -- 添加医生详细信息
   INSERT INTO doctors (user_id, name, department, title, working_hours) 
   VALUES (@new_doctor_user_id, '新医生', 'Emergency', '住院医师', '周一至周日 24小时');
   
   -- 添加患者详细信息
   INSERT INTO patients (user_id, name, gender, birth_date, id_number, phone_number) 
   VALUES (@new_patient_user_id, '新患者', 'Female', '1995-06-15', '110101199506150099', '13900139999');
   ```

## 📊 **对前端UI查询的影响**

### **影响说明**

| 数据库状态 | 医生数量 | 患者数量 | 用户总数 | 影响 |
|------------|----------|----------|----------|------|
| **仅基础数据** | 5个 | 8个 | 13个 | ✅ 数据固定，适合UI |
| **运行API测试后** | 6个 | 9个 | 15个 | ⚠️ 数据增加，可能影响UI |

### **具体影响**

1. **数据数量变化**：
   - 前端UI如果硬编码期望"5个医生，8个患者"，会出现不一致
   - 分页、统计功能可能显示错误的数量

2. **ID不连续**：
   - API测试创建的用户ID可能不连续（如：user_id = 14, 15）
   - 可能影响基于ID的查询逻辑

3. **测试数据混合**：
   - 真实业务数据与测试数据混合
   - 可能影响数据的一致性和可预测性

## 🎯 **解决方案**

### **方案1：使用纯净数据库（推荐）**

```bash
# 使用专门为前端UI设计的纯净数据库
mysql -u root -p < sql/clean_database_setup.sql
```

**特点**：
- ✅ 数据数量固定：5个医生，8个患者
- ✅ 不包含API测试数据
- ✅ 适合前端UI开发和测试
- ✅ 数据可预测，ID连续

### **方案2：分离测试环境**

```bash
# 创建专门的API测试数据库
mysql -u root -p -e "CREATE DATABASE hospital_api_test_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;"

# 在API测试数据库中运行测试
mysql -u root -p hospital_api_test_db < sql/hospital_complete_setup.sql
mysql -u root -p hospital_api_test_db < sql/api_test_examples.sql

# 前端UI使用纯净数据库
mysql -u root -p hospital_db < sql/clean_database_setup.sql
```

### **方案3：重置数据库**

```bash
# 如果已经运行了API测试，可以重置数据库
mysql -u root -p -e "DROP DATABASE IF EXISTS hospital_db;"
mysql -u root -p < sql/clean_database_setup.sql
```

## 📋 **最佳实践建议**

### **开发阶段**
- **前端UI开发**：使用 `sql/clean_database_setup.sql`
- **后端API开发**：使用 `sql/hospital_complete_setup.sql`
- **API测试**：在单独的测试数据库中运行

### **测试阶段**
- **单元测试**：使用最小化的测试数据
- **集成测试**：使用完整的测试环境
- **UI测试**：使用固定的纯净数据

### **生产阶段**
- **初始化**：使用纯净的表结构（不含测试数据）
- **数据迁移**：确保数据的一致性和完整性

## 🔍 **验证方法**

### **检查当前数据状态**
```sql
-- 检查用户数量
SELECT 
    user_type,
    COUNT(*) as count
FROM users 
GROUP BY user_type;

-- 检查是否有API测试数据
SELECT * FROM users WHERE username IN ('dr_new', 'patient_new');
```

### **确认数据纯净性**
```sql
-- 预期结果（纯净数据库）
-- Doctor: 5
-- Patient: 8
-- Total: 13

-- 如果看到Doctor: 6, Patient: 9，说明包含了API测试数据
```

## 💡 **总结**

**API测试中的"动态创建用户"**是指测试脚本在运行过程中会向数据库插入新的测试数据，这会改变数据库中的记录数量，从而影响前端UI的查询结果。

**建议**：
- 前端UI开发使用 `sql/clean_database_setup.sql`
- API功能测试使用单独的测试数据库
- 保持开发、测试、生产环境的数据一致性