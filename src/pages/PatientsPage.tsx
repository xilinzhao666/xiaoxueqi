import React, { useEffect, useState } from 'react'
import { useSupabase } from '../contexts/SupabaseContext'
import { Users, Search, Plus, Phone, Mail, Calendar } from 'lucide-react'
import { format, parseISO } from 'date-fns'

interface Patient {
  patient_id: number
  user_id: number
  name: string
  gender: 'Male' | 'Female'
  birth_date: string
  id_number: string
  phone_number: string | null
  users?: {
    email: string | null
    phone_number: string | null
  }
}

const PatientsPage: React.FC = () => {
  const [patients, setPatients] = useState<Patient[]>([])
  const [loading, setLoading] = useState(true)
  const [searchTerm, setSearchTerm] = useState('')
  const [selectedGender, setSelectedGender] = useState('')
  const supabase = useSupabase()

  useEffect(() => {
    fetchPatients()
  }, [])

  const fetchPatients = async () => {
    try {
      const { data, error } = await supabase
        .from('patients')
        .select(`
          *,
          users (
            email,
            phone_number
          )
        `)
        .order('name')

      if (error) throw error
      setPatients(data || [])
    } catch (error) {
      console.error('Error fetching patients:', error)
    } finally {
      setLoading(false)
    }
  }

  const calculateAge = (birthDate: string) => {
    const birth = parseISO(birthDate)
    const today = new Date()
    let age = today.getFullYear() - birth.getFullYear()
    const monthDiff = today.getMonth() - birth.getMonth()
    
    if (monthDiff < 0 || (monthDiff === 0 && today.getDate() < birth.getDate())) {
      age--
    }
    
    return age
  }

  const filteredPatients = patients.filter(patient => {
    const matchesSearch = patient.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         patient.id_number.includes(searchTerm) ||
                         (patient.phone_number && patient.phone_number.includes(searchTerm))
    
    const matchesGender = !selectedGender || patient.gender === selectedGender
    
    return matchesSearch && matchesGender
  })

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
          <h1 className="text-3xl font-bold text-gray-900 mb-2">患者管理</h1>
          <p className="text-gray-600">管理医院患者信息</p>
        </div>
        <button className="btn btn-primary">
          <Plus className="w-4 h-4 mr-2" />
          添加患者
        </button>
      </div>

      <div className="card">
        <div className="flex flex-col sm:flex-row gap-4 mb-6">
          <div className="relative flex-1">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400 w-4 h-4" />
            <input
              type="text"
              placeholder="搜索患者姓名、身份证号或电话..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="input pl-10 w-full"
            />
          </div>
          <select
            value={selectedGender}
            onChange={(e) => setSelectedGender(e.target.value)}
            className="select w-full sm:w-32"
          >
            <option value="">所有性别</option>
            <option value="Male">男</option>
            <option value="Female">女</option>
          </select>
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
          {filteredPatients.map((patient) => (
            <div key={patient.patient_id} className="bg-gray-50 rounded-xl p-6 hover:shadow-md transition-shadow">
              <div className="flex items-start space-x-4">
                <div className="w-12 h-12 bg-purple-100 rounded-xl flex items-center justify-center">
                  <Users className="w-6 h-6 text-purple-600" />
                </div>
                <div className="flex-1 min-w-0">
                  <h3 className="text-lg font-semibold text-gray-900 mb-1">{patient.name}</h3>
                  <div className="flex items-center space-x-4 text-sm text-gray-600 mb-2">
                    <span>{patient.gender === 'Male' ? '男' : '女'}</span>
                    <span>{calculateAge(patient.birth_date)}岁</span>
                  </div>
                  <p className="text-xs text-gray-500 mb-3 font-mono">{patient.id_number}</p>
                  
                  <div className="space-y-1">
                    {patient.users?.email && (
                      <div className="flex items-center space-x-2 text-xs text-gray-600">
                        <Mail className="w-3 h-3" />
                        <span className="truncate">{patient.users.email}</span>
                      </div>
                    )}
                    {(patient.phone_number || patient.users?.phone_number) && (
                      <div className="flex items-center space-x-2 text-xs text-gray-600">
                        <Phone className="w-3 h-3" />
                        <span>{patient.phone_number || patient.users?.phone_number}</span>
                      </div>
                    )}
                    <div className="flex items-center space-x-2 text-xs text-gray-600">
                      <Calendar className="w-3 h-3" />
                      <span>{format(parseISO(patient.birth_date), 'yyyy-MM-dd')}</span>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          ))}
        </div>

        {filteredPatients.length === 0 && (
          <div className="text-center py-12">
            <Users className="w-12 h-12 text-gray-400 mx-auto mb-4" />
            <h3 className="text-lg font-medium text-gray-900 mb-2">未找到患者</h3>
            <p className="text-gray-600">请尝试调整搜索条件</p>
          </div>
        )}
      </div>
    </div>
  )
}

export default PatientsPage