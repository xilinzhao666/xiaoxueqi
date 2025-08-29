import React, { useEffect, useState } from 'react'
import { useSupabase } from '../contexts/SupabaseContext'
import { UserCheck, Search, Plus, Phone, Mail } from 'lucide-react'

interface Doctor {
  doctor_id: number
  user_id: number
  name: string
  department: string
  title: string | null
  working_hours: string
  profile_picture: string | null
  users?: {
    email: string | null
    phone_number: string | null
  }
}

const DoctorsPage: React.FC = () => {
  const [doctors, setDoctors] = useState<Doctor[]>([])
  const [loading, setLoading] = useState(true)
  const [searchTerm, setSearchTerm] = useState('')
  const [selectedDepartment, setSelectedDepartment] = useState('')
  const supabase = useSupabase()

  useEffect(() => {
    fetchDoctors()
  }, [])

  const fetchDoctors = async () => {
    try {
      const { data, error } = await supabase
        .from('doctors')
        .select(`
          *,
          users (
            email,
            phone_number
          )
        `)
        .order('name')

      if (error) throw error
      setDoctors(data || [])
    } catch (error) {
      console.error('Error fetching doctors:', error)
    } finally {
      setLoading(false)
    }
  }

  const filteredDoctors = doctors.filter(doctor => {
    const matchesSearch = doctor.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         doctor.department.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         (doctor.title && doctor.title.toLowerCase().includes(searchTerm.toLowerCase()))
    
    const matchesDepartment = !selectedDepartment || doctor.department === selectedDepartment
    
    return matchesSearch && matchesDepartment
  })

  const departments = [...new Set(doctors.map(d => d.department))]

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
          <h1 className="text-3xl font-bold text-gray-900 mb-2">医生管理</h1>
          <p className="text-gray-600">管理医院医生信息</p>
        </div>
        <button className="btn btn-primary">
          <Plus className="w-4 h-4 mr-2" />
          添加医生
        </button>
      </div>

      <div className="card">
        <div className="flex flex-col sm:flex-row gap-4 mb-6">
          <div className="relative flex-1">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400 w-4 h-4" />
            <input
              type="text"
              placeholder="搜索医生姓名、科室或职称..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="input pl-10 w-full"
            />
          </div>
          <select
            value={selectedDepartment}
            onChange={(e) => setSelectedDepartment(e.target.value)}
            className="select w-full sm:w-48"
          >
            <option value="">所有科室</option>
            {departments.map(dept => (
              <option key={dept} value={dept}>{dept}</option>
            ))}
          </select>
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
          {filteredDoctors.map((doctor) => (
            <div key={doctor.doctor_id} className="bg-gray-50 rounded-xl p-6 hover:shadow-md transition-shadow">
              <div className="flex items-start space-x-4">
                <div className="w-12 h-12 bg-primary-100 rounded-xl flex items-center justify-center">
                  <UserCheck className="w-6 h-6 text-primary-600" />
                </div>
                <div className="flex-1 min-w-0">
                  <h3 className="text-lg font-semibold text-gray-900 mb-1">{doctor.name}</h3>
                  <p className="text-sm text-primary-600 font-medium mb-2">{doctor.department}</p>
                  {doctor.title && (
                    <p className="text-sm text-gray-600 mb-2">{doctor.title}</p>
                  )}
                  <p className="text-xs text-gray-500 mb-3">{doctor.working_hours}</p>
                  
                  <div className="space-y-1">
                    {doctor.users?.email && (
                      <div className="flex items-center space-x-2 text-xs text-gray-600">
                        <Mail className="w-3 h-3" />
                        <span>{doctor.users.email}</span>
                      </div>
                    )}
                    {doctor.users?.phone_number && (
                      <div className="flex items-center space-x-2 text-xs text-gray-600">
                        <Phone className="w-3 h-3" />
                        <span>{doctor.users.phone_number}</span>
                      </div>
                    )}
                  </div>
                </div>
              </div>
            </div>
          ))}
        </div>

        {filteredDoctors.length === 0 && (
          <div className="text-center py-12">
            <UserCheck className="w-12 h-12 text-gray-400 mx-auto mb-4" />
            <h3 className="text-lg font-medium text-gray-900 mb-2">未找到医生</h3>
            <p className="text-gray-600">请尝试调整搜索条件</p>
          </div>
        )}
      </div>
    </div>
  )
}

export default DoctorsPage