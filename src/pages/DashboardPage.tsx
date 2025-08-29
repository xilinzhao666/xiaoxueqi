import React, { useEffect, useState } from 'react'
import { useSupabase } from '../contexts/SupabaseContext'
import { Users, UserCheck, Calendar, FileText, TrendingUp, Activity } from 'lucide-react'

interface HospitalStats {
  totalUsers: number
  totalDoctors: number
  totalPatients: number
  totalCases: number
  totalAppointments: number
  bookedAppointments: number
  attendedAppointments: number
  cancelledAppointments: number
  totalHospitalizations: number
  totalPrescriptions: number
  totalMedications: number
}

const DashboardPage: React.FC = () => {
  const [stats, setStats] = useState<HospitalStats | null>(null)
  const [loading, setLoading] = useState(true)
  const supabase = useSupabase()

  useEffect(() => {
    fetchStats()
  }, [])

  const fetchStats = async () => {
    try {
      const [
        usersResult,
        doctorsResult,
        patientsResult,
        casesResult,
        appointmentsResult,
        hospitalizationResult,
        prescriptionsResult,
        medicationsResult
      ] = await Promise.all([
        supabase.from('users').select('*', { count: 'exact', head: true }),
        supabase.from('doctors').select('*', { count: 'exact', head: true }),
        supabase.from('patients').select('*', { count: 'exact', head: true }),
        supabase.from('cases').select('*', { count: 'exact', head: true }),
        supabase.from('appointments').select('*', { count: 'exact', head: true }),
        supabase.from('hospitalization').select('*', { count: 'exact', head: true }),
        supabase.from('prescriptions').select('*', { count: 'exact', head: true }),
        supabase.from('medications').select('*', { count: 'exact', head: true })
      ])

      const appointmentsByStatus = await supabase
        .from('appointments')
        .select('status')

      const bookedCount = appointmentsByStatus.data?.filter(a => a.status === 'Booked').length || 0
      const attendedCount = appointmentsByStatus.data?.filter(a => a.status === 'Attended').length || 0
      const cancelledCount = appointmentsByStatus.data?.filter(a => a.status === 'Cancelled').length || 0

      setStats({
        totalUsers: usersResult.count || 0,
        totalDoctors: doctorsResult.count || 0,
        totalPatients: patientsResult.count || 0,
        totalCases: casesResult.count || 0,
        totalAppointments: appointmentsResult.count || 0,
        bookedAppointments: bookedCount,
        attendedAppointments: attendedCount,
        cancelledAppointments: cancelledCount,
        totalHospitalizations: hospitalizationResult.count || 0,
        totalPrescriptions: prescriptionsResult.count || 0,
        totalMedications: medicationsResult.count || 0,
      })
    } catch (error) {
      console.error('Error fetching stats:', error)
    } finally {
      setLoading(false)
    }
  }

  if (loading) {
    return (
      <div className="flex items-center justify-center h-64">
        <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-primary-600"></div>
      </div>
    )
  }

  const statCards = [
    {
      title: '总用户数',
      value: stats?.totalUsers || 0,
      icon: Users,
      color: 'bg-blue-500',
      bgColor: 'bg-blue-50',
      textColor: 'text-blue-700'
    },
    {
      title: '医生数量',
      value: stats?.totalDoctors || 0,
      icon: UserCheck,
      color: 'bg-green-500',
      bgColor: 'bg-green-50',
      textColor: 'text-green-700'
    },
    {
      title: '患者数量',
      value: stats?.totalPatients || 0,
      icon: Users,
      color: 'bg-purple-500',
      bgColor: 'bg-purple-50',
      textColor: 'text-purple-700'
    },
    {
      title: '病例数量',
      value: stats?.totalCases || 0,
      icon: FileText,
      color: 'bg-orange-500',
      bgColor: 'bg-orange-50',
      textColor: 'text-orange-700'
    },
    {
      title: '预约总数',
      value: stats?.totalAppointments || 0,
      icon: Calendar,
      color: 'bg-indigo-500',
      bgColor: 'bg-indigo-50',
      textColor: 'text-indigo-700'
    },
    {
      title: '住院记录',
      value: stats?.totalHospitalizations || 0,
      icon: Activity,
      color: 'bg-red-500',
      bgColor: 'bg-red-50',
      textColor: 'text-red-700'
    }
  ]

  return (
    <div className="space-y-8">
      <div>
        <h1 className="text-3xl font-bold text-gray-900 mb-2">仪表板</h1>
        <p className="text-gray-600">医院管理系统概览</p>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
        {statCards.map((card) => (
          <div key={card.title} className="card hover:shadow-md transition-shadow">
            <div className="flex items-center justify-between">
              <div>
                <p className="text-sm font-medium text-gray-600 mb-1">{card.title}</p>
                <p className="text-3xl font-bold text-gray-900">{card.value}</p>
              </div>
              <div className={`p-3 rounded-xl ${card.bgColor}`}>
                <card.icon className={`w-6 h-6 ${card.textColor}`} />
              </div>
            </div>
          </div>
        ))}
      </div>

      {stats && (
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          <div className="card">
            <h3 className="text-lg font-semibold text-gray-900 mb-4">预约状态分布</h3>
            <div className="space-y-3">
              <div className="flex items-center justify-between">
                <span className="text-sm text-gray-600">已预约</span>
                <div className="flex items-center space-x-2">
                  <div className="w-24 bg-gray-200 rounded-full h-2">
                    <div 
                      className="bg-blue-500 h-2 rounded-full" 
                      style={{ width: `${(stats.bookedAppointments / stats.totalAppointments) * 100}%` }}
                    ></div>
                  </div>
                  <span className="text-sm font-medium">{stats.bookedAppointments}</span>
                </div>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm text-gray-600">已就诊</span>
                <div className="flex items-center space-x-2">
                  <div className="w-24 bg-gray-200 rounded-full h-2">
                    <div 
                      className="bg-green-500 h-2 rounded-full" 
                      style={{ width: `${(stats.attendedAppointments / stats.totalAppointments) * 100}%` }}
                    ></div>
                  </div>
                  <span className="text-sm font-medium">{stats.attendedAppointments}</span>
                </div>
              </div>
              <div className="flex items-center justify-between">
                <span className="text-sm text-gray-600">已取消</span>
                <div className="flex items-center space-x-2">
                  <div className="w-24 bg-gray-200 rounded-full h-2">
                    <div 
                      className="bg-red-500 h-2 rounded-full" 
                      style={{ width: `${(stats.cancelledAppointments / stats.totalAppointments) * 100}%` }}
                    ></div>
                  </div>
                  <span className="text-sm font-medium">{stats.cancelledAppointments}</span>
                </div>
              </div>
            </div>
          </div>

          <div className="card">
            <h3 className="text-lg font-semibold text-gray-900 mb-4">系统活动</h3>
            <div className="space-y-4">
              <div className="flex items-center space-x-3">
                <div className="w-2 h-2 bg-green-500 rounded-full"></div>
                <span className="text-sm text-gray-600">系统运行正常</span>
              </div>
              <div className="flex items-center space-x-3">
                <div className="w-2 h-2 bg-blue-500 rounded-full"></div>
                <span className="text-sm text-gray-600">数据库连接正常</span>
              </div>
              <div className="flex items-center space-x-3">
                <div className="w-2 h-2 bg-purple-500 rounded-full"></div>
                <span className="text-sm text-gray-600">
                  总处方数: {stats.totalPrescriptions}
                </span>
              </div>
              <div className="flex items-center space-x-3">
                <div className="w-2 h-2 bg-orange-500 rounded-full"></div>
                <span className="text-sm text-gray-600">
                  总药物记录: {stats.totalMedications}
                </span>
              </div>
            </div>
          </div>
        </div>
      )}
    </div>
  )
}

export default LoginPage