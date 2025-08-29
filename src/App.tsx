import React from 'react'
import { Routes, Route, Navigate } from 'react-router-dom'
import { AuthProvider } from './contexts/AuthContext'
import { SupabaseProvider } from './contexts/SupabaseContext'
import Layout from './components/Layout'
import LoginPage from './pages/LoginPage'
import DashboardPage from './pages/DashboardPage'
import DoctorsPage from './pages/DoctorsPage'
import PatientsPage from './pages/PatientsPage'
import AppointmentsPage from './pages/AppointmentsPage'
import CasesPage from './pages/CasesPage'
import PrescriptionsPage from './pages/PrescriptionsPage'
import HospitalizationPage from './pages/HospitalizationPage'
import ProtectedRoute from './components/ProtectedRoute'

function App() {
  return (
    <SupabaseProvider>
      <AuthProvider>
        <Routes>
          <Route path="/login" element={<LoginPage />} />
          <Route path="/" element={
            <ProtectedRoute>
              <Layout />
            </ProtectedRoute>
          }>
            <Route index element={<Navigate to="/dashboard" replace />} />
            <Route path="dashboard" element={<DashboardPage />} />
            <Route path="doctors" element={<DoctorsPage />} />
            <Route path="patients" element={<PatientsPage />} />
            <Route path="appointments" element={<AppointmentsPage />} />
            <Route path="cases" element={<CasesPage />} />
            <Route path="prescriptions" element={<PrescriptionsPage />} />
            <Route path="hospitalization" element={<HospitalizationPage />} />
          </Route>
        </Routes>
      </AuthProvider>
    </SupabaseProvider>
  )
}

export default App