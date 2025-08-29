import React, { useEffect, useState } from 'react'
import { useSupabase } from '../contexts/SupabaseContext'
import { Calendar, Search, Plus, Clock, User, UserCheck } from 'lucide-react'
import { format, parseISO } from 'date-fns'

interface Appointment {
  appointment_id: number
  patient_id: number
  doctor_id: number
  appointment_time: string
  department: string
  status: 'Booked' | 'Attended' | 'Cancelled'
  patients?: {
    name: string
  }
  doctors?: {
    name: string
  }
}

const AppointmentsPage: React.FC = () => {
  const [appointments, setAppointments] = useState<Appointment[]>([])
  const [loading, setLoading] = useState(true)
  const [searchTerm, setSearchTerm] = useState('')
  const [selectedStatus, setSelectedStatus] = useState('')
  const [selectedDepartment, setSelectedDepartment] = useState('')
  const supabase = useSupabase()

  useEffect(() => {
    fetchAppointments()
  }, [])

  const fetchAppointments = async () => {
    try {
      const { data, error } = await supabase
        .from('appointments')
        .select(`
          *,
          patients (
            name
          ),
          doctors (
            name
          )
        `)
        .order('appointment_time', { ascending: false })

      if (error) throw error
      setAppointments(data || [])
    } catch (error) {
      console.error('Error fetching appointments:', error)
    } finally {
      setLoading(false)
    }
  }

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'Booked': return 'bg-blue-100 text-blue-800'
      case 'Attended': return 'bg-green-100 text-green-800'
      case 'Cancelled': return 'bg-red-100 text-red-800'
      default: return 'bg-gray-100 text-gray-800'
    }
  }

  const getStatusText = (status: string) => {
    switch (status) {
      case 'Booked': return '已预约'
      case 'Attended': return '已就诊'
      case 'Cancelled': return '已取消'
      default: return status
    }
  }

  const filteredAppointments = appointments.filter(appointment => {
    const matchesSearch = appointment.patients?.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         appointment.doctors?.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         appointment.department.toLowerCase().includes(searchTerm.toLowerCase())
    
    const matchesStatus = !selectedStatus || appointment.status === selectedStatus
    const matchesDepartment = !selectedDepartment || appointment.department === selectedDepartment
    
    return matchesSearch && matchesStatus && matchesDepartment
  })

  const departments = [...new Set(appointments.map(a => a.department))]

  if (loading) {
    return (
      <div className="flex items-center justify-center h-64">
        <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-primary-600"></div>
      </div>
    )
  }

  return (
    <div className="space-y-6">
      <div className="flex items-center justify-between">
        <div>
          <h1 className="text-3xl font-bold text-gray-900 mb-2">预约管理</h1>
          <p className="text-gray-600">管理患者预约信息</p>
        </div>
        <button className="btn btn-primary">
          <Plus className="w-4 h-4 mr-2" />
          新建预约
        </button>
      </div>

      <div className="card">
        <div className="flex flex-col lg:flex-row gap-4 mb-6">
          <div className="relative flex-1">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400 w-4 h-4" />
            <input
              type="text"
              placeholder="搜索患者、医生或科室..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="input pl-10 w-full"
            />
          </div>
          <select
            value={selectedStatus}
            onChange={(e) => setSelectedStatus(e.target.value)}
            className="select w-full lg:w-32"
          >
            <option value="">所有状态</option>
            <option value="Booked">已预约</option>
            <option value="Attended">已就诊</option>
            <option value="Cancelled">已取消</option>
          </select>
          <select
            value={selectedDepartment}
            onChange={(e) => setSelectedDepartment(e.target.value)}
            className="select w-full lg:w-48"
          >
            <option value="">所有科室</option>
            {departments.map(dept => (
              <option key={dept} value={dept}>{dept}</option>
            ))}
          </select>
        </div>

        <div className="space-y-4">
          {filteredAppointments.map((appointment) => (
            <div key={appointment.appointment_id} className="bg-white border border-gray-200 rounded-xl p-6 hover:shadow-sm transition-shadow">
              <div className="flex items-center justify-between">
                <div className="flex items-center space-x-4">
                  <div className="w-12 h-12 bg-indigo-100 rounded-xl flex items-center justify-center">
                    <Calendar className="w-6 h-6 text-indigo-600" />
                  </div>
                  <div>
                    <h3 className="text-lg font-semibold text-gray-900 mb-1">
                      预约 #{appointment.appointment_id}
                    </h3>
                    <div className="flex items-center space-x-4 text-sm text-gray-600">
                      <div className="flex items-center space-x-1">
                        <User className="w-4 h-4" />
                        <span>{appointment.patients?.name}</span>
                      </div>
                      <div className="flex items-center space-x-1">
                        <UserCheck className="w-4 h-4" />
                        <span>{appointment.doctors?.name}</span>
                      </div>
                      <div className="flex items-center space-x-1">
                        <Clock className="w-4 h-4" />
                        <span>{format(parseISO(appointment.appointment_time), 'yyyy-MM-dd HH:mm')}</span>
                      </div>
                    </div>
                  </div>
                </div>
                
                <div className="flex items-center space-x-3">
                  <span className="text-sm font-medium text-gray-600">{appointment.department}</span>
                  <span className={`px-3 py-1 rounded-full text-xs font-medium ${getStatusColor(appointment.status)}`}>
                    {getStatusText(appointment.status)}
                  </span>
                </div>
              </div>
            </div>
          ))}
        </div>

        {filteredAppointments.length === 0 && (
          <div className="text-center py-12">
            <Calendar className="w-12 h-12 text-gray-400 mx-auto mb-4" />
            <h3 className="text-lg font-medium text-gray-900 mb-2">未找到预约</h3>
            <p className="text-gray-600">请尝试调整搜索条件</p>
          </div>
        )}
      </div>
    </div>
  )
}

export default AppointmentsPage