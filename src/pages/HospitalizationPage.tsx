import React, { useEffect, useState } from 'react'
import { useSupabase } from '../contexts/SupabaseContext'
import { Building2, Search, Plus, User, Calendar, MapPin, UserCheck } from 'lucide-react'
import { format, parseISO, differenceInDays } from 'date-fns'

interface Hospitalization {
  hospitalization_id: number
  patient_id: number
  ward_number: string
  bed_number: string
  admission_date: string
  attending_doctor: string
  patients?: {
    name: string
    id_number: string
    phone_number: string | null
  }
}

const HospitalizationPage: React.FC = () => {
  const [hospitalizations, setHospitalizations] = useState<Hospitalization[]>([])
  const [loading, setLoading] = useState(true)
  const [searchTerm, setSearchTerm] = useState('')
  const [selectedWard, setSelectedWard] = useState('')
  const supabase = useSupabase()

  useEffect(() => {
    fetchHospitalizations()
  }, [])

  const fetchHospitalizations = async () => {
    try {
      const { data, error } = await supabase
        .from('hospitalization')
        .select(`
          *,
          patients (
            name,
            id_number,
            phone_number
          )
        `)
        .order('admission_date', { ascending: false })

      if (error) throw error
      setHospitalizations(data || [])
    } catch (error) {
      console.error('Error fetching hospitalizations:', error)
    } finally {
      setLoading(false)
    }
  }

  const calculateDaysHospitalized = (admissionDate: string) => {
    return differenceInDays(new Date(), parseISO(admissionDate))
  }

  const filteredHospitalizations = hospitalizations.filter(hospitalization => {
    const matchesSearch = hospitalization.patients?.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         hospitalization.ward_number.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         hospitalization.bed_number.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         hospitalization.attending_doctor.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         hospitalization.patients?.id_number.includes(searchTerm)
    
    const matchesWard = !selectedWard || hospitalization.ward_number === selectedWard
    
    return matchesSearch && matchesWard
  })

  const wards = [...new Set(hospitalizations.map(h => h.ward_number))]

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
          <h1 className="text-3xl font-bold text-gray-900 mb-2">住院管理</h1>
          <p className="text-gray-600">管理患者住院信息</p>
        </div>
        <button className="btn btn-primary">
          <Plus className="w-4 h-4 mr-2" />
          办理住院
        </button>
      </div>

      <div className="card">
        <div className="flex flex-col sm:flex-row gap-4 mb-6">
          <div className="relative flex-1">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400 w-4 h-4" />
            <input
              type="text"
              placeholder="搜索患者、病房、床位或主治医生..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="input pl-10 w-full"
            />
          </div>
          <select
            value={selectedWard}
            onChange={(e) => setSelectedWard(e.target.value)}
            className="select w-full sm:w-32"
          >
            <option value="">所有病房</option>
            {wards.map(ward => (
              <option key={ward} value={ward}>{ward}</option>
            ))}
          </select>
        </div>

        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          {filteredHospitalizations.map((hospitalization) => (
            <div key={hospitalization.hospitalization_id} className="bg-white border border-gray-200 rounded-xl p-6 hover:shadow-sm transition-shadow">
              <div className="flex items-start space-x-4">
                <div className="w-12 h-12 bg-red-100 rounded-xl flex items-center justify-center">
                  <Building2 className="w-6 h-6 text-red-600" />
                </div>
                <div className="flex-1">
                  <div className="flex items-center justify-between mb-3">
                    <h3 className="text-lg font-semibold text-gray-900">
                      {hospitalization.patients?.name}
                    </h3>
                    <span className="text-sm font-medium text-red-600 bg-red-50 px-3 py-1 rounded-full">
                      住院中
                    </span>
                  </div>
                  
                  <div className="space-y-2 mb-4">
                    <div className="flex items-center space-x-2 text-sm text-gray-600">
                      <MapPin className="w-4 h-4" />
                      <span>病房: {hospitalization.ward_number} - 床位: {hospitalization.bed_number}</span>
                    </div>
                    <div className="flex items-center space-x-2 text-sm text-gray-600">
                      <UserCheck className="w-4 h-4" />
                      <span>主治医生: {hospitalization.attending_doctor}</span>
                    </div>
                    <div className="flex items-center space-x-2 text-sm text-gray-600">
                      <Calendar className="w-4 h-4" />
                      <span>入院时间: {format(parseISO(hospitalization.admission_date), 'yyyy-MM-dd HH:mm')}</span>
                    </div>
                    <div className="text-sm text-gray-600 font-mono">
                      身份证: {hospitalization.patients?.id_number}
                    </div>
                  </div>
                  
                  <div className="flex items-center justify-between pt-3 border-t border-gray-200">
                    <span className="text-sm text-gray-600">
                      住院天数: {calculateDaysHospitalized(hospitalization.admission_date)} 天
                    </span>
                    {hospitalization.patients?.phone_number && (
                      <span className="text-sm text-gray-600">
                        电话: {hospitalization.patients.phone_number}
                      </span>
                    )}
                  </div>
                </div>
              </div>
            </div>
          ))}
        </div>

        {filteredHospitalizations.length === 0 && (
          <div className="text-center py-12">
            <Building2 className="w-12 h-12 text-gray-400 mx-auto mb-4" />
            <h3 className="text-lg font-medium text-gray-900 mb-2">未找到住院记录</h3>
            <p className="text-gray-600">请尝试调整搜索条件</p>
          </div>
        )}
      </div>
    </div>
  )
}

export default HospitalizationPage