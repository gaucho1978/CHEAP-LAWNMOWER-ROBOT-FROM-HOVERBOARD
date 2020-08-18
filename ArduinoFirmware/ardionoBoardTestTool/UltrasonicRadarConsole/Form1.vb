Imports System.IO.Ports


Public Class Form1
    Dim myPenRed As Pen = New Pen(Drawing.Color.Red, 10)
    Dim myPenGreen As Pen = New Pen(Drawing.Color.Green, 10)
    Dim myPen As Pen = New Pen(Drawing.Color.Gray, 10)
    Dim myPenBlack As Pen = New Pen(Drawing.Color.Black, 3)

    Dim rect150 As New Rectangle(50, 50, 300, 300)
    Dim rect130 As New Rectangle(60, 60, 280, 280)
    Dim rect110 As New Rectangle(70, 70, 260, 260)

    Dim com1 As IO.Ports.SerialPort = Nothing
    'Dim myBrush As Brush = New SolidBrush(Color.Coral)
    Dim myGraphics As Graphics

    Dim DistanceCmLastMeasure(8) As Byte


    Dim roverShape() As Point
    Dim comPort As String
    Dim sensorsRectangleArray() As Rectangle
    Dim sensorsStartAngleArray() As String
    Dim sensorsShape() As Point
    Dim sensorLabel() As Label


    Private Sub Timer1_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Timer1.Tick
        readSerialLine()

        Refresh()

        'draw sensors arcs
        Dim curSensor As Byte = 0
        Dim curRange As Integer = 0
        For curSensor = 0 To sensorsRectangleArray.Length - 1 Step 1
            
            For curRange = 20 To 250 Step 10
                If (DistanceCmLastMeasure(curSensor) > curRange) Then
                    myPen.Color = Color.Green
                Else
                    myPen.Color = Color.Red
                End If
                myGraphics.DrawArc(myPen, sensorsRectangleArray(curSensor).X - curRange + sensorsRectangleArray(curSensor).Width * 0.34F, sensorsRectangleArray(curSensor).Y - curRange + sensorsRectangleArray(curSensor).Height * 0.34F, sensorsRectangleArray(curSensor).Width * 0.383333325F + ((curRange - 20) * 2), sensorsRectangleArray(curSensor).Height * 0.383333325F + ((curRange - 20) * 2), CSng(sensorsStartAngleArray(curSensor)) - 22, 45)
            Next
            myGraphics.DrawPie(myPenBlack, sensorsRectangleArray(curSensor).X - sensorsRectangleArray(curSensor).Width / 1.95F, sensorsRectangleArray(curSensor).Y - sensorsRectangleArray(curSensor).Height / 1.95F, sensorsRectangleArray(curSensor).Width * 1.95F, sensorsRectangleArray(curSensor).Height * 1.95F, CSng(sensorsStartAngleArray(curSensor)) - 22, 45)
            sensorLabel(curSensor).Location = New System.Drawing.Point(sensorsRectangleArray(curSensor).X + sensorsRectangleArray(curSensor).Width * 0.36F, sensorsRectangleArray(curSensor).Y + sensorsRectangleArray(curSensor).Height * 0.4F)

        Next

        'draw rover shape
        myGraphics.DrawLines(myPenBlack, roverShape)



        Application.DoEvents()
    End Sub

    Public Sub readSerialLine()
        Try
            Application.DoEvents()
            Dim Buffer() As Byte = Read(com1, 10)
            com1.DiscardInBuffer()
            'Dim Incoming As String = com1.ReadLine()
            If Buffer.Length >= 1 Then
                Dim tmpStr As String = "Received: "

                For i As Integer = 0 To Buffer.Length - 1 Step 1
                    tmpStr = tmpStr & Hex(Buffer(i)) & " "
                    If i = 10 Then Exit For
                Next
                RichTextBox1.AppendText(tmpStr & vbCrLf)

                'let's decode received data
                'evaluate crc
                Dim evaluatedCrc As UShort = calcrcUshort(0, Buffer, 8)
                If (evaluatedCrc = BytesToUShort(Buffer(8), Buffer(9))) Then
                    RichTextBox1.AppendText("Received Checksum is Correct" & vbCrLf)
                Else
                    RichTextBox1.AppendText("Received Checksum is Wrong. Message discarded!" & vbCrLf)
                    Return
                End If
                'checksum is correct, save and show data
                For i As Integer = 0 To 7 Step 1
                    DistanceCmLastMeasure(i) = Buffer(i)
                    sensorLabel(i).Text = DistanceCmLastMeasure(i)
                Next

 
            End If


        Catch ex As TimeoutException
            RichTextBox1.AppendText("Error: Serial Port read timed out." & vbCrLf)
            If com1 IsNot Nothing Then com1.Close()
            Application.DoEvents()
        Catch ex As Exception
            RichTextBox1.AppendText("Error.: " & ex.Message & vbCrLf)
            Application.DoEvents()
        Finally


        End Try

    End Sub

    Private Function BytesToUShort(ByVal HiByte As Byte, ByVal LoByte As Byte) As UShort
        
        Return 256 * HiByte + LoByte


    End Function
    Public Function Read(ByVal port As SerialPort, ByVal count As Integer) As Byte()
        Dim buffer(count - 1) As Byte
        Dim readBytes As Integer
        Dim totalReadBytes As Integer
        Dim offset As Integer
        Dim remaining As Integer = count

        Try
            Do
                readBytes = port.Read(buffer, offset, remaining)
                offset += readBytes
                remaining -= readBytes
                totalReadBytes += readBytes
            Loop While remaining > 0 AndAlso readBytes > 0
        Catch ex As TimeoutException
            ReDim Preserve buffer(totalReadBytes - 1)
        End Try

        Return buffer

    End Function

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        LoadSettings()
        myGraphics = Me.CreateGraphics
        GetSerialPortNames()
        ComboBox1.SelectedIndex = ComboBox1.FindString(comPort)

    End Sub

    Sub GetSerialPortNames()
        ' Show all available COM ports.
        For Each sp As String In My.Computer.Ports.SerialPortNames
            ComboBox1.Items.Add(sp)
        Next
    End Sub


    Private Sub ComboBox1_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ComboBox1.SelectedIndexChanged

        Try
            If com1 IsNot Nothing Then
                If (com1.IsOpen) Then
                    com1.Close()
                End If
            End If
            com1 = My.Computer.Ports.OpenSerialPort(My.Computer.Ports.SerialPortNames(ComboBox1.SelectedIndex), 9600)
            com1.ReadTimeout = 100


        Catch ex As TimeoutException
            MsgBox("Error: Timeout")
            If com1 IsNot Nothing Then com1.Close()
        Catch ex As Exception
            MsgBox("Error: " & ex.Message)
            If com1 IsNot Nothing Then com1.Close()
        Finally
            RichTextBox1.AppendText("Port Successfully open" & vbCrLf)
        End Try
    End Sub

    Private Sub RichTextBox1_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles RichTextBox1.TextChanged
        RichTextBox1.SelectionStart = RichTextBox1.Text.Length

        RichTextBox1.ScrollToCaret()
    End Sub


    Function calcrcUshort(ByVal crc As UShort, ByVal data() As Byte, ByVal count As UShort) As UShort
        ' crc usually initialized as &HFFFF ???
        For Each b As Byte In data

            If count = 0 Then Exit For
            count = count - 1
            Dim d As UShort = CUShort(b)
            crc = crc Xor (d << 8)
            For j = 0 To 7
                If ((crc And &H8000) <> 0) Then
                    crc = (crc << 1) Xor &H1021
                Else
                    crc = (crc << 1)
                End If
            Next
        Next
        Return crc And &HFFFF
    End Function

    Public Sub LoadSettings()
        Dim fileNumber = FreeFile()
        FileOpen(fileNumber, "settings.txt", OpenMode.Input, OpenAccess.Read, OpenShare.Shared)
        Dim tmpStr As String
        Dim index As Byte = 0

        
        While Not EOF(fileNumber)
            tmpStr = FileSystem.LineInput(fileNumber)
            If (tmpStr = "" Or tmpStr.StartsWith("#")) Then
                'avoid this line empty or comment
                Continue While
            End If
            'acquire this line

            Select index
                Case 0 'com port
                    index = index + 1
                    comPort = tmpStr

                Case 1 'rover shape
                    index = index + 1
                    Dim tmpArr() = tmpStr.Split(",")
                    For i As Integer = 0 To tmpArr.Length / 2 - 1 Step 1
                        ReDim Preserve roverShape(i)
                        roverShape(i).X = tmpArr(i * 2)
                        roverShape(i).Y = tmpArr(i * 2 + 1)
                    Next
                Case Else

                    index = index + 1
                    Dim tmpArr() = tmpStr.Split(",")
                    If tmpArr.Length < 5 Then Continue While
                    ReDim Preserve sensorsRectangleArray(index - 3)
                    ReDim Preserve sensorsStartAngleArray(index - 3)
                    ReDim Preserve sensorLabel(index - 3)

                    sensorsRectangleArray(index - 3) = New Rectangle(tmpArr(0), tmpArr(1), tmpArr(2), tmpArr(3))
                    sensorsStartAngleArray(index - 3) = tmpArr(4)
                    sensorLabel(index - 3) = New Label()
                    sensorLabel(index - 3).AutoSize = True
                    sensorLabel(index - 3).Font = New Font(sensorLabel(index - 3).Font.FontFamily, 20)
                    sensorLabel(index - 3).BorderStyle = BorderStyle.FixedSingle
                    sensorLabel(index - 3).Text = "---"
                    Me.Controls.Add(sensorLabel(index - 3))



            End Select


        End While




    End Sub
End Class
