def fact(n)
 if n==0
    return 1
 else
    return n*fact(n-1)
 end
end

class CloseListener 
 include java.awt.event.ActionListener
 def actionPerformed(event)
    puts "CloseListere.actionPerformed() called"
    java.lang.System.exit(0)
 end
end

def showFactInWindow(title,number)
    f = fact(number)
    frame = javax.swing.JFrame.new(title)
    frame.setLayout(java.awt.FlowLayout.new())
    button = javax.swing.JButton.new("Close")
    button.addActionListener(CloseListener.new)
    frame.contentPane.add(javax.swing.JLabel.new(number.to_s+"! = "+f.to_s))
    frame.contentPane.add(button)
    frame.defaultCloseOperation=javax.swing.WindowConstants::EXIT_ON_CLOSE
    frame.pack()
    frame.visible=true
    return f
end

