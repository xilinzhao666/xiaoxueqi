export interface Database {
  public: {
    Tables: {
      users: {
        Row: {
          user_id: number
          username: string
          password: string
          user_type: 'Doctor' | 'Patient'
          email: string | null
          phone_number: string | null
          created_at: string
        }
        Insert: {
          user_id?: number
          username: string
          password: string
          user_type: 'Doctor' | 'Patient'
          email?: string | null
          phone_number?: string | null
          created_at?: string
        }
        Update: {
          user_id?: number
          username?: string
          password?: string
          user_type?: 'Doctor' | 'Patient'
          email?: string | null
          phone_number?: string | null
          created_at?: string
        }
      }
      doctors: {
        Row: {
          doctor_id: number
          user_id: number
          name: string
          department: string
          title: string | null
          working_hours: string
          profile_picture: string | null
        }
        Insert: {
          doctor_id?: number
          user_id: number
          name: string
          department: string
          title?: string | null
          working_hours: string
          profile_picture?: string | null
        }
        Update: {
          doctor_id?: number
          user_id?: number
          name?: string
          department?: string
          title?: string | null
          working_hours?: string
          profile_picture?: string | null
        }
      }
      patients: {
        Row: {
          patient_id: number
          user_id: number
          name: string
          gender: 'Male' | 'Female'
          birth_date: string
          id_number: string
          phone_number: string | null
        }
        Insert: {
          patient_id?: number
          user_id: number
          name: string
          gender: 'Male' | 'Female'
          birth_date: string
          id_number: string
          phone_number?: string | null
        }
        Update: {
          patient_id?: number
          user_id?: number
          name?: string
          gender?: 'Male' | 'Female'
          birth_date?: string
          id_number?: string
          phone_number?: string | null
        }
      }
      cases: {
        Row: {
          case_id: number
          patient_id: number
          department: string
          doctor_id: number
          diagnosis: string
          diagnosis_date: string
        }
        Insert: {
          case_id?: number
          patient_id: number
          department: string
          doctor_id: number
          diagnosis: string
          diagnosis_date?: string
        }
        Update: {
          case_id?: number
          patient_id?: number
          department?: string
          doctor_id?: number
          diagnosis?: string
          diagnosis_date?: string
        }
      }
      appointments: {
        Row: {
          appointment_id: number
          patient_id: number
          doctor_id: number
          appointment_time: string
          department: string
          status: 'Booked' | 'Attended' | 'Cancelled'
        }
        Insert: {
          appointment_id?: number
          patient_id: number
          doctor_id: number
          appointment_time: string
          department: string
          status?: 'Booked' | 'Attended' | 'Cancelled'
        }
        Update: {
          appointment_id?: number
          patient_id?: number
          doctor_id?: number
          appointment_time?: string
          department?: string
          status?: 'Booked' | 'Attended' | 'Cancelled'
        }
      }
      hospitalization: {
        Row: {
          hospitalization_id: number
          patient_id: number
          ward_number: string
          bed_number: string
          admission_date: string
          attending_doctor: string
        }
        Insert: {
          hospitalization_id?: number
          patient_id: number
          ward_number: string
          bed_number: string
          admission_date?: string
          attending_doctor: string
        }
        Update: {
          hospitalization_id?: number
          patient_id?: number
          ward_number?: string
          bed_number?: string
          admission_date?: string
          attending_doctor?: string
        }
      }
      prescriptions: {
        Row: {
          prescription_id: number
          case_id: number
          doctor_id: number
          prescription_content: string
          issued_date: string
        }
        Insert: {
          prescription_id?: number
          case_id: number
          doctor_id: number
          prescription_content: string
          issued_date?: string
        }
        Update: {
          prescription_id?: number
          case_id?: number
          doctor_id?: number
          prescription_content?: string
          issued_date?: string
        }
      }
      medications: {
        Row: {
          medication_id: number
          prescription_id: number
          medication_name: string
          quantity: number
          usage_instructions: string
        }
        Insert: {
          medication_id?: number
          prescription_id: number
          medication_name: string
          quantity: number
          usage_instructions: string
        }
        Update: {
          medication_id?: number
          prescription_id?: number
          medication_name?: string
          quantity?: number
          usage_instructions?: string
        }
      }
    }
  }
}