import React, { useEffect, useState } from 'react'
import { useSupabase } from '../contexts/SupabaseContext'
import { Pill, Search, Plus, User, UserCheck, Calendar, FileText } from 'lucide-react'
import { format, parseISO } from 'date-fns'

interface Prescription {
  prescription_id: number
  case_id: number
  doctor_id: number
  prescription_content: string
  issued_date: string
  doctors?: {
    name: string
  }
  cases?: {
    patients?: {
      name: string
    }
  }
  medications?: Array<{
    medication_name: string
    quantity: number
    usage_instructions: string
  }>
}

const PrescriptionsPage: React.FC = () => {
  const [prescriptions, setPrescriptions] = useState<Prescription[]>([])
  const [loading, setLoading] = useState(true)
  const [searchTerm, setSearchTerm] = useState('')
  const supabase = useSupabase()

  useEffect(() => {
    fetchPrescriptions()
  }, [])

  const fetchPrescriptions = async () => {
    try {
      const { data, error } = await supabase
        .from('prescriptions')
        .select(`
          *,
          doctors (
            name
          ),
          cases (
            patients (
              name
            )
          ),
          medications (
            medication_name,
            quantity,
            usage_instructions
          )
        `)
        .order('issued_date', { ascending: false })

      if (error) throw error
      setPrescriptions(data || [])
    } catch (error) {
      console.error('Error fetching prescriptions:', error)
    } finally {
      setLoading(false)
    }
  }

  const filteredPrescriptions = prescriptions.filter(prescription => {
    const matchesSearch = prescription.doctors?.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         prescription.cases?.patients?.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         prescription.prescription_content.toLowerCase().includes(searchTerm.toLowerCase()) ||
                         prescription.medications?.some(med => 
                           med.medication_name.toLowerCase().includes(searchTerm.toLowerCase())
                         )
    
    return matchesSearch
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
          <h1 className="text-3xl font-bold text-gray-900 mb-2">处方管理</h1>
          <p className="text-gray-600">管理医生开具的处方</p>
        </div>
        <button className="btn btn-primary">
          <Plus className="w-4 h-4 mr-2" />
          开具处方
        </button>
      </div>

      <div className="card">
        <div className="mb-6">
          <div className="relative">
            <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 text-gray-400 w-4 h-4" />
            <input
              type="text"
              placeholder="搜索患者、医生、处方内容或药物..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="input pl-10 w-full"
            />
          </div>
        </div>

        <div className="space-y-6">
          {filteredPrescriptions.map((prescription) => (
            <div key={prescription.prescription_id} className="bg-white border border-gray-200 rounded-xl p-6 hover:shadow-sm transition-shadow">
              <div className="flex items-start space-x-4">
                <div className="w-12 h-12 bg-green-100 rounded-xl flex items-center justify-center">
                  <Pill className="w-6 h-6 text-green-600" />
                </div>
                <div className="flex-1">
                  <div className="flex items-center justify-between mb-3">
                    <h3 className="text-lg font-semibold text-gray-900">
                      处方 #{prescription.prescription_id}
                    </h3>
                    <div className="flex items-center space-x-2 text-sm text-gray-600">
                      <Calendar className="w-4 h-4" />
                      <span>{format(parseISO(prescription.issued_date), 'yyyy-MM-dd HH:mm')}</span>
                    </div>
                  </div>
                  
                  <div className="grid grid-cols-1 md:grid-cols-2 gap-4 mb-4">
                    <div className="flex items-center space-x-2 text-sm text-gray-600">
                      <User className="w-4 h-4" />
                      <span>患者: {prescription.cases?.patients?.name}</span>
                    </div>
                    <div className="flex items-center space-x-2 text-sm text-gray-600">
                      <UserCheck className="w-4 h-4" />
                      <span>医生: {prescription.doctors?.name}</span>
                    </div>
                  </div>
                  
                  <div className="bg-gray-50 rounded-lg p-4 mb-4">
                    <h4 className="text-sm font-medium text-gray-900 mb-2 flex items-center">
                      <FileText className="w-4 h-4 mr-2" />
                      处方内容
                    </h4>
                    <p className="text-sm text-gray-700 leading-relaxed">{prescription.prescription_content}</p>
                  </div>

                  {prescription.medications && prescription.medications.length > 0 && (
                    <div className="bg-blue-50 rounded-lg p-4">
                      <h4 className="text-sm font-medium text-gray-900 mb-3 flex items-center">
                        <Pill className="w-4 h-4 mr-2" />
                        药物清单
                      </h4>
                      <div className="space-y-3">
                        {prescription.medications.map((medication, index) => (
                          <div key={index} className="bg-white rounded-lg p-3 border border-blue-200">
                            <div className="flex items-center justify-between mb-2">
                              <h5 className="font-medium text-gray-900">{medication.medication_name}</h5>
                              <span className="text-sm text-blue-600 bg-blue-100 px-2 py-1 rounded">
                                数量: {medication.quantity}
                              </span>
                            </div>
                            <p className="text-sm text-gray-600">{medication.usage_instructions}</p>
                          </div>
                        ))}
                      </div>
                    </div>
                  )}
                </div>
              </div>
            </div>
          ))}
        </div>

        {filteredPrescriptions.length === 0 && (
          <div className="text-center py-12">
            <Pill className="w-12 h-12 text-gray-400 mx-auto mb-4" />
            <h3 className="text-lg font-medium text-gray-900 mb-2">未找到处方</h3>
            <p className="text-gray-600">请尝试调整搜索条件</p>
          </div>
        )}
      </div>
    </div>
  )
}

export default PrescriptionsPage