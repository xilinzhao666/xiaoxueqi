import React, { useEffect, useState } from 'react'
import { useSupabase } from '../contexts/SupabaseContext'
import { FileText, Search, Plus, User, UserCheck, Calendar } from 'lucide-react'
import { format, parseISO } from 'date-fns'

interface Case {
  case_id: number
  patient_id: number
  department: string
  doctor_id: number
  diagnosis: string
  diagnosis_date: string
  patients?: {
    name: string
    id_number: string
  }
  doctors?: {
    name: string
  }
}

const CasesPage: React.FC = () => {
  const [cases, setCases] = useState<Case[]>([])
  const [loading, setLoading] = useState(true)
  const [searchTerm, setSearchTerm] = useState('')
  const [selectedDepartment, setSelectedDepartment] = useState('')
  const supabase = useSupabase()

  useEffect(() => {
    fetchCases()
  }, [])

  const fetchCases = async () => {
    try {
      const { data, error } = await supabase
        .from('cases')
        .select(`
          *,
          patients (
            name,
            id_number
          ),
          doctors (
            name
          )
        `)
        .order('diagnosis_date', { ascending: false })

      if (error) throw error
      setCases(data || [])
    } catch (error) {
      console.error('Error fetching cases:', error)
    } finally {
      setLoading(false)
    }
  }

  const filteredCases = cases.filter(medicalCase => {
    const matchesSearch = medicalCase.patients?.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         medicalCase.doctors?.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         medicalCase.diagnosis.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         medicalCase.patients?.id_number.includes(searchTerm)
    
    const matchesDepartment = !selectedDepartment || medicalCase.department === selectedDepartment
    
    return matchesSearch && matchesDepartment
  })

  const departments = [...new Set(cases.map(c => c.department))]

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
          <h1 className="text-3xl font-bold text-gray-900 mb-2">病例管理</h1>
          <p className="text-gray-600">管理患者病例信息</p>
        </div>
        <button className="btn btn-primary">
          <Plus className="w-4 h-4 mr-2" />
          新建病例
        </button>
      </div>

      <div className="card">
        <div className="flex flex-col sm:flex-row gap-4 mb-6">
          <div className="relative flex-1">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400 w-4 h-4" />
            <input
              type="text"
              placeholder="搜索患者、医生、诊断或身份证号..."
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

        <div className="space-y-4">
          {filteredCases.map((medicalCase) => (
            <div key={medicalCase.case_id} className="bg-white border border-gray-200 rounded-xl p-6 hover:shadow-sm transition-shadow">
              <div className="flex items-start space-x-4">
                <div className="w-12 h-12 bg-orange-100 rounded-xl flex items-center justify-center">
                  <FileText className="w-6 h-6 text-orange-600" />
                </div>
                <div className="flex-1">
                  <div className="flex items-center justify-between mb-3">
                    <h3 className="text-lg font-semibold text-gray-900">
                      病例 #{medicalCase.case_id}
                    </h3>
                    <span className="text-sm font-medium text-primary-600 bg-primary-50 px-3 py-1 rounded-full">
                      {medicalCase.department}
                    </span>
                  </div>
                  
                  <div className="grid grid-cols-1 md:grid-cols-2 gap-4 mb-4">
                    <div className="flex items-center space-x-2 text-sm text-gray-600">
                      <User className="w-4 h-4" />
                      <span>患者: {medicalCase.patients?.name}</span>
                    </div>
                    <div className="flex items-center space-x-2 text-sm text-gray-600">
                      <UserCheck className="w-4 h-4" />
                      <span>医生: {medicalCase.doctors?.name}</span>
                    </div>
                    <div className="flex items-center space-x-2 text-sm text-gray-600">
                      <Calendar className="w-4 h-4" />
                      <span>诊断时间: {format(parseISO(medicalCase.diagnosis_date), 'yyyy-MM-dd HH:mm')}</span>
                    </div>
                    <div className="text-sm text-gray-600 font-mono">
                      ID: {medicalCase.patients?.id_number}
                    </div>
                  </div>
                  
                  <div className="bg-gray-50 rounded-lg p-4">
                    <h4 className="text-sm font-medium text-gray-900 mb-2">诊断结果</h4>
                    <p className="text-sm text-gray-700 leading-relaxed">{medicalCase.diagnosis}</p>
                  </div>
                </div>
              </div>
            </div>
          ))}
        </div>

        {filteredCases.length === 0 && (
          <div className="text-center py-12">
            <FileText className="w-12 h-12 text-gray-400 mx-auto mb-4" />
            <h3 className="text-lg font-medium text-gray-900 mb-2">未找到病例</h3>
            <p className="text-gray-600">请尝试调整搜索条件</p>
          </div>
        )}
      </div>
    </div>
  )
}

export default CasesPage