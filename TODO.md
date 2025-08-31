notice: token全是ApiHandler::generateToken根据相关信息随机生成的，同样的信息每次也生成不同的token，未存入db中
1. public.schedule.list: handlePublicScheduleList中模糊搜索未实现
2. patient.auth.register: handlePatientRegister中verificationCode验证码只是输入没有检验
3. patient.auth.resetPassword: handlePatientResetPassword中的验证码只是检验是否6位没有确认正确性，重置密码部分需要管理员权限，目前是创建了一个新用户而不是修改以前用户的密码
4. patient.profile.get: 目前是创建新的UserSession，由于没有存储token，导致运行返回认证令牌无效或已过期
5. patient.profile.update: 目前是创建新的UserSession，由于没有存储token，导致运行返回认证令牌无效或已过期
6. patient.appointment.create: token导致验证不通过
7. patient.medicalRecord.list：token
8. patient.prescription.list: token
9. patient.prescription.get: token
10. patient.chat.sendMessage: token
